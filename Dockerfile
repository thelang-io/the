FROM ubuntu

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y build-essential cmake git && \
    apt-get autoclean && \
    apt-get autoremove && \
    apt-get clean && \
    rm -rf /tmp/* /var/lib/apt/lists/* /var/tmp/*

WORKDIR /app
COPY . .
RUN cmake .
RUN cmake --build .

ENTRYPOINT ["./the-core"]
