#include <stdint.h>
#include "fat16.h"
#include "status.h"
#include "string/string.h"
#include "disk/disk.h"
#include "disk/streamer.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "kernel.h"

#define ALPHAOS_FAT16_SIGNATURE 0x29
#define ALPHAOS_FAT16_FAT_ENTRY_SIZE 0x02
#define ALPHAOS_FAT16_BAD_SECTOR 0xFF7
#define ALPHAOS_FAT16_UNUSED 0x00

typedef unsigned int FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1

// Fat directory entry attributes bitmask
#define FAT_FILE_READ_ONLY 0x01
#define FAT_FILE_HIDDEN 0x02
#define FAT_FILE_SYSTEM 0x04
#define FAT_FILE_VOLUME_LABEL 0x08
#define FAT_FILE_SUBDIRECTORY 0x10
#define FAT_FILE_ARCHIVE 0x20
#define FAT_FILE_DEVICE 0x40
#define FAT_FILE_RESERVED 0x80

struct fat_header_extended
{
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

struct fat_header
{
    uint8_t short_jmp_ins[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t sectors_big;
} __attribute__((packed));

struct fat_h
{
    struct fat_header primary_header;
    union fat_h_e
    {
        struct fat_header_extended extended_header;
    } shared;
};

struct fat_directory_item
{
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t file_size;
} __attribute__((packed));

struct fat_directory
{
    struct fat_directory_item *item;
    int total;
    int sector_pos;
    int ending_sector_pos;
};

struct fat_item
{
    union
    {
        struct fat_directory_item *item;
        struct fat_directory *directory;
    };

    FAT_ITEM_TYPE type;
};

struct fat_file_descriptor
{
    struct fat_item *item;
    uint32_t pos;
};

struct fat_private
{
    struct fat_h header;
    struct fat_directory root_directory;

    // Used to stream data clusters
    struct disk_stream *cluster_read_stream;
    // Used to stream the file allocation table
    struct disk_stream *fat_read_stream;

    // Used in situation where we stream the directory
    struct disk_stream *directory_stream;
};

int fat16_resolve(struct disk *disk);
void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode);
int fat16_read(struct disk *disk, void *descriptor, uint32_t size, uint32_t nmemb, char *out_ptr);
int fat16_seek(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);
int fat16_stat(struct disk *disk, void *private, struct file_stat *stat);
int fat16_close(void *private);

struct filesystem fat16_fs =
{
    .resolve = fat16_resolve,
    .open = fat16_open,
    .read = fat16_read,
    .seek = fat16_seek,
    .stat = fat16_stat,
    .close = fat16_close,
};

struct filesystem *fat16_init()
{
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

static void fat16_init_private(struct disk *disk, struct fat_private *private)
{
    memset(private, 0, sizeof(struct fat_private));
    private->cluster_read_stream = diskstreamer_new(disk->id);
    private->fat_read_stream = diskstreamer_new(disk->id);
    private->directory_stream = diskstreamer_new(disk->id);
}

int fat16_sector_to_absolute(struct disk *disk, int sector)
{
    return sector * disk->sector_size;
}

int fat16_get_total_items_for_directory(struct disk *disk, uint32_t directory_start_sector)
{
    struct fat_directory_item item;
    struct fat_directory_item empty_item;
    memset(&empty_item, 0, sizeof(empty_item));

    struct fat_private *fat_private = disk->fs_private;

    int res = 0;
    int i = 0;
    int directory_start_pos = directory_start_sector * disk->sector_size;
    struct disk_stream *stream = fat_private->directory_stream;
    if (diskstreamer_seek(stream, directory_start_pos) != ALPHAOS_ALL_OK) {
        res = -EIO;
        goto out;
    }

    while(1) {
        if (diskstreamer_read(stream, &item, sizeof(item)) != ALPHAOS_ALL_OK) {
            res = -EIO;
            goto out;
        }

        if (item.filename[0] == 0x00) {
            break;
        }

        // Is the item unused
        if (item.filename[0] == 0xE5) {
            continue;
        }

        i++;
    }

    res = i;

out:
    return res;
}

int fat16_get_root_directory(struct disk *disk, struct fat_private *fat_private, struct fat_directory *directory)
{
    int res = 0;
    struct fat_directory_item *dir = 0;
    struct fat_header *primary_header = &fat_private->header.primary_header;
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    int root_dir_entries = fat_private->header.primary_header.root_dir_entries;
    int root_dir_size = (root_dir_entries * sizeof(struct fat_directory_item));
    int total_sectors = root_dir_size / disk->sector_size;

    if (root_dir_size % disk->sector_size) {
        total_sectors++;
    }

    int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

    dir = kzalloc(root_dir_size);
    if (!dir) {
        res = -ENOMEM;
        goto out;
    }

    struct disk_stream *stream = fat_private->directory_stream;
    if (diskstreamer_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != ALPHAOS_ALL_OK) {
        res = -EIO;
        goto out;
    }

    if (diskstreamer_read(stream, dir, root_dir_size) != ALPHAOS_ALL_OK) {
        res = -EIO;
        goto out;
    }

    directory->item = dir;
    directory->total = total_items;
    directory->sector_pos = root_dir_sector_pos;
    directory->ending_sector_pos = root_dir_sector_pos + total_sectors;

out:
    if (res != ALPHAOS_ALL_OK) {
        if (dir) {
            kfree(dir);
        }
    }
    return res;
}

int fat16_resolve(struct disk *disk)
{
    int res = 0;
    struct fat_private *fat_private = kzalloc(sizeof(struct fat_private));
    fat16_init_private(disk, fat_private);

    disk->fs_private = fat_private;
    disk->filesystem = &fat16_fs;

    struct disk_stream *stream = diskstreamer_new(disk->id);
    if (!stream) {
        res = -ENOMEM;
        goto out;
    }

    if (diskstreamer_read(stream, &fat_private->header, sizeof(fat_private->header)) != ALPHAOS_ALL_OK) {
        res = -EIO;
        goto out;
    }

    if (fat_private->header.shared.extended_header.signature != 0x29) {
        res = -EFSNOTUS;
        goto out;
    }

    if (fat16_get_root_directory(disk, fat_private, &fat_private->root_directory) != ALPHAOS_ALL_OK) {
        res = -EIO;
        goto out;
    }

out:
    if (stream) {
        diskstreamer_close(stream);
    }

    if (res < 0) {
        if (fat_private) {
            kfree(fat_private);
        }
        disk->fs_private = 0;
    }
    return res;
}

void FAT16_to_proper_string(char **out, const char *in)
{
    while (*in != 0 && *in != ' ') {
        **out = *in;
        *out += 1;
        in++;
    }

    if (*in == ' ') {
        **out = 0;
    }
}

void fat16_get_full_relative_filename(struct fat_directory_item *item, char *out, int max_len)
{
    memset(out, 0, max_len);
    char *out_tmp = out;
    FAT16_to_proper_string(&out_tmp, (const char *) item->filename);
    if (item->ext[0] != 0 && item->ext[0] != ' ') {
        *out_tmp++ = '.';
        FAT16_to_proper_string(&out_tmp, (const char *) item->ext);
    }
}

struct fat_directory_item *fat16_clone_directory_item(struct fat_directory_item *item, int size)
{
    struct fat_directory_item *new_item = 0;

    if (size < sizeof(struct fat_directory_item)) {
        return 0;
    }

    new_item = kzalloc(size);

    if (!new_item) {
        return 0;
    }

    memcpy(new_item, item, size);

    return new_item;
}

static uint32_t fat16_get_first_cluster(struct fat_directory_item *item)
{
    return (item->high_16_bits_first_cluster) | item->low_16_bits_first_cluster;
}

static int fat16_cluster_to_sector(struct fat_private *private, int cluster)
{
    return private->root_directory.ending_sector_pos + ((cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private *private)
{
    return private->header.primary_header.reserved_sectors;
}

static int fat16_get_fat_entry(struct disk *disk, int cluster)
{
    int res = -1;
    struct fat_private *private = disk->fs_private;
    struct disk_stream *stream = private->fat_read_stream;
    if (!stream) {
        res = -EIO;
        goto out;
    }

    uint32_t fat_table_position = fat16_get_first_fat_sector(private) * disk->sector_size;
    res = diskstreamer_seek(stream, fat_table_position * (cluster * ALPHAOS_FAT16_FAT_ENTRY_SIZE));
    if (res < 0) {
        goto out;
    }

    uint16_t result = 0;
    res = diskstreamer_read(stream, &result, sizeof(result));
    if (res < 0) {
        goto out;
    }

    res = result;

out:
    return res;
}

/**
 * @brief Gets the correct cluster to use based on the starting cluster and the offset
 */
static int fat16_get_cluster_for_offet(struct disk *disk, int starting_cluster, int offset)
{
    int res = 0;
    struct fat_private *private = disk->fs_private;
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = starting_cluster;
    int cluster_ahead = offset / size_of_cluster_bytes;
    for (int i = 0; i < cluster_ahead; i++) {
        int entry = fat16_get_fat_entry(disk, cluster_to_use);
        if (entry == 0xFF8 || entry == 0xFFF) {
            // last entry of the file
            res = -EIO;
            goto out;
        }

        // Sector is marked as bad
        if (entry == ALPHAOS_FAT16_BAD_SECTOR) {
            res = -EIO;
            goto out;
        }

        // Reserved sector
        if (entry == 0xFF0 || entry == 0xFF6) {
            res = -EIO;
            goto out;
        }

        if (entry == 0) {
            res = -EIO;
            goto out;
        }

        cluster_to_use = entry;
    }

    res = cluster_to_use;

out:
    return res;
}

static int fat16_read_internal_from_stream(struct disk *disk, struct disk_stream *stream, int cluster, int offset, int total, void *out)
{
    int res = 0;
    struct fat_private *private = disk->fs_private;
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = fat16_get_cluster_for_offet(disk, cluster, offset);
    if (cluster_to_use < 0) {
        res = cluster_to_use;
        goto out;
    }

    int offset_from_cluster = offset % size_of_cluster_bytes;

    int starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
    int starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
    int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
    res = diskstreamer_seek(stream, starting_pos);
    if (res != ALPHAOS_ALL_OK) {
        goto out;
    }

    res = diskstreamer_read(stream, out, total_to_read);
    if (res != ALPHAOS_ALL_OK) {
        goto out;
    }

    total -= total_to_read;
    if (total > 0) {
        // still have more to read
        res = fat16_read_internal_from_stream(disk, stream, cluster, offset + total_to_read, total, out + total_to_read);
    }

out:
    return res;
}

static int fat16_read_internal(struct disk *disk, int starting_cluster, int offset, int total, void *out)
{
    struct fat_private *fs_private = disk->fs_private;
    struct disk_stream *stream = fs_private->cluster_read_stream;
    return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

void fat16_free_directory(struct fat_directory *directory)
{
    if (!directory) {
        return;
    }

    if (directory->item) {
        kfree(directory->item);
    }

    kfree(directory);
}

void fat_16_fat_item_free(struct fat_item *item)
{
    if (!item) {
        return;
    }

    if (item->type == FAT_ITEM_TYPE_DIRECTORY) {
        fat16_free_directory(item->directory);
    } else if (item->type == FAT_ITEM_TYPE_FILE) {
        kfree(item->item);
    }

    kfree(item);
}

struct fat_directory *fat16_load_fat_directory(struct disk *disk, struct fat_directory_item *item)
{
    int res = 0;
    struct fat_directory *directory = 0;
    struct fat_private *fat_private = disk->fs_private;
    if (!(item->attributes & FAT_FILE_SUBDIRECTORY)) {
        res = -EINVARG;
        goto out;
    }

    directory = kzalloc(sizeof(struct fat_directory));
    if (!directory) {
        res = -ENOMEM;
        goto out;
    }

    int cluster = fat16_get_first_cluster(item);
    int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
    directory->total = total_items;
    int directory_size = directory->total * sizeof(struct fat_directory_item);
    directory->item = kzalloc(directory_size);
    if (!directory->item) {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);
    if (res != ALPHAOS_ALL_OK) {
        res = -EIO;
        goto out;
    }

out:
    if (res != ALPHAOS_ALL_OK) {
        fat16_free_directory(directory);
    }
    return directory;
}

struct fat_item *fat16_new_fat_item_for_directory_item(struct disk *disk, struct fat_directory_item *item)
{
    struct fat_item *f_item = kzalloc(sizeof(struct fat_item));
    if (!f_item) {
        return 0;
    }

    if (item->attributes & FAT_FILE_SUBDIRECTORY) {
        f_item->directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;
        return f_item;
    }

    f_item->type = FAT_ITEM_TYPE_FILE;
    f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item));

    return f_item;
}

struct fat_item *fat16_find_item_in_directory(struct disk *disk, struct fat_directory *directory, const char *name)
{
    struct fat_item *f_item = 0;
    char tmp_filename[ALPHAOS_MAX_PATH];
    for (int i = 0; i < directory->total; i++) {
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
        if (istrncmp(tmp_filename, name, sizeof(tmp_filename)) == 0) {
            f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
        }
    }

    return f_item;
}

struct fat_item *fat16_get_directory_entry(struct disk *disk, struct path_part *path)
{
    struct fat_private *fat_private = disk->fs_private;
    struct fat_item *current_item = 0;
    struct fat_item *root_item = fat16_find_item_in_directory(disk, &fat_private->root_directory, path->part);
    if (!root_item) {
        goto out;
    }

    struct path_part *next_part = path->next;
    current_item = root_item;
    while (next_part != 0) {
        if (current_item->type != FAT_ITEM_TYPE_DIRECTORY) {
            current_item = 0;
            break;
        }

        struct fat_item *tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next_part->part);
        fat_16_fat_item_free(current_item);


        current_item = tmp_item;
        next_part = next_part->next;
    }

out:
    return current_item;
}

void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode)
{
    struct fat_file_descriptor *descriptor = 0;
    int res = 0;

    if (mode != FILE_MODE_READ) {
        res = -ERDONLY;
        goto out;
    }

    descriptor = kzalloc(sizeof(struct fat_file_descriptor));
    if (!descriptor) {
        res = -ENOMEM;
        goto out;
    }

    descriptor->item = fat16_get_directory_entry(disk, path);
    if (!descriptor->item) {
        res = -EIO;
        goto out;
    }

    descriptor->pos = 0;

    return descriptor;

out:
    if (descriptor) {
        kfree(descriptor);
    }

    return ERROR(res);
}

static int fat16_free_file_descriptor(struct fat_file_descriptor *descriptor)
{
    if (!descriptor) {
        return -EINVARG;
    }

    if (descriptor->item) {
        fat_16_fat_item_free(descriptor->item);
    }

    kfree(descriptor);
    return 0;
}

int fat16_close(void *private)
{
    fat16_free_file_descriptor((struct fat_file_descriptor *) private);
    return 0;
}

int fat16_stat(struct disk *disk, void *private, struct file_stat *stat)
{
    int res = 0;
    struct fat_file_descriptor *descriptor = (struct fat_file_descriptor *) private;
    struct fat_item *item = descriptor->item;
    if (item->type != FAT_ITEM_TYPE_FILE) {
        res = -EINVARG;
        goto out;
    }

    struct fat_directory_item *ritem = item->item;
    stat->filesize = ritem->file_size;
    stat->flags = 0x00;

    if (ritem->attributes & FAT_FILE_READ_ONLY) {
        stat->flags |= FILE_STAT_READ_ONLY;
    }

out:
    return res;
}

int fat16_read(struct disk *disk, void *descriptor, uint32_t size, uint32_t nmemb, char *out_ptr)
{
    int res = 0;
    struct fat_file_descriptor *fat_desc = descriptor;
    struct fat_directory_item *item = fat_desc->item->item;
    int offset = fat_desc->pos;

    for (uint32_t i = 0; i < nmemb; i++) {
        res = fat16_read_internal(disk, fat16_get_first_cluster(item), offset, size, out_ptr);
        if (ISERR(res)) {
            goto out;
        }

        offset += size;
        out_ptr += size;
    }

    res = nmemb;

out:
    return res;
}

int fat16_seek(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode)
{
    int res = 0;
    struct fat_file_descriptor *desc = private;
    struct fat_item *desc_item = desc->item;

    if (desc_item->type != FAT_ITEM_TYPE_FILE) {
        res = -EINVARG;
        goto out;
    }

    struct fat_directory_item *ritem = desc_item->item;
    if (offset >= ritem->file_size) {
        res = -EIO;
        goto out;
    }

    switch (seek_mode) {
        case SEEK_SET:
            desc->pos = offset;
            break;
        case SEEK_END:
            res = -EUNIMP;
            break;
        case SEEK_CUR:
            desc->pos += offset;
            break;
        default:
            res = -EINVARG;
            break;
    }

out:
    return res;
}
