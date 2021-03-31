FROM ubuntu

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y build-essential cmake git valgrind && \
    apt-get autoclean && \
    apt-get autoremove && \
    apt-get clean && \
    rm -rf /tmp/* /var/lib/apt/lists/* /var/tmp/*

WORKDIR /app
COPY . .
RUN mkdir -p build
RUN (cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON)
RUN cmake --build build

ENTRYPOINT ["scripts/docker-entrypoint.sh"]
