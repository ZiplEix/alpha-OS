ENTRY(_start)
OUTPUT_FORMAT("binary")
SECTIONS
{
    . = 1M; /* Set the code starting address to 1M */
    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }
}
