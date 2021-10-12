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
RUN make

ENTRYPOINT ["valgrind", "--error-exitcode=1", "--leak-check=full", "--show-leak-kinds=all", "--tool=memcheck", "--track-origins=yes", "build/the-core"]
