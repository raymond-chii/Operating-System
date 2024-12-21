FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    gcc \
    strace

WORKDIR /app

COPY hello.c .

RUN gcc -o hello hello.c

CMD ["strace", "./hello"]