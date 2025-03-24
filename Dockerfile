FROM zipleix/i686-gcc-crosscompiler:latest

WORKDIR /app

RUN apt update && apt install -y sudo

COPY . .

RUN ls -la /home/opt/cross

CMD ["make", "compile"]
