FROM --platform=linux/arm64 ubuntu:latest

RUN apt-get update && apt-get install -y \
    gcc \
    binutils \
    strace

WORKDIR /app

COPY hello.s .

RUN as -o hello.o hello.s && \
    ld -o hello hello.o

CMD ["strace", "./hello"]