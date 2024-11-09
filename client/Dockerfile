FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    libboost-all-dev \
    libssl-dev \
    libpthread-stubs0-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN g++ -o WebSocketClient main.cpp -lboost_system -lssl -lcrypto -lpthread

CMD ["./WebSocketClient"]
