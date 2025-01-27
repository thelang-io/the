#
# Copyright (c) 2018 Aaron Delasy
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && \
    apt-get install -y build-essential clang curl cmake git ninja-build valgrind && \
    apt-get autoclean && \
    apt-get autoremove && \
    apt-get clean && \
    rm -rf /tmp/* /var/lib/apt/lists/* /var/tmp/*

RUN curl -fsSL https://cdn.thelang.io/deps.tar.gz | tar -xz
ENV DEPS_DIR="$PWD/the/native/linux"

WORKDIR /app
COPY . .
RUN cmake . -G Ninja -D BUILD_TESTS=ON -D TEST_CODEGEN_MEMCHECK=ON
RUN cmake --build .

ENTRYPOINT ["ctest", "--output-on-failure"]
