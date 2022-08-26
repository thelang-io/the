FROM ubuntu

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y build-essential cmake git ninja-build valgrind && \
    apt-get autoclean && \
    apt-get autoremove && \
    apt-get clean && \
    rm -rf /tmp/* /var/lib/apt/lists/* /var/tmp/*

WORKDIR /app
COPY . .
RUN cmake . -G "Ninja" -D BUILD_TESTS=ON -D TEST_CODEGEN_MEMCHECK=ON
RUN cmake --build .

ENTRYPOINT ["ctest", "--output-on-failure"]
