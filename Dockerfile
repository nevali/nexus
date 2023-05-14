FROM debian:bullseye-slim AS base

FROM base AS buildbase

RUN apt-get update -qq && apt-get -q -y install make clang automake autoconf libtool pkg-config libjansson-dev libssl-dev

ENV CC="clang"
ENV CXX="clang++"
ENV CPPFLAGS="-g -O0 -W -Wall -Werror"

FROM buildbase AS builder

RUN mkdir -p /src
WORKDIR /src
COPY . /src

RUN autoreconf -fvi && ./configure --prefix=/nexus && make clean && make && make check && make install

FROM buildbase AS devcontainer

RUN apt-get install -qq -y valgrind gdb git procps nano locales-all
VOLUME /workspace
WORKDIR /workspace

FROM base AS runtime

RUN apt-get update -qq && apt-get install -qq -y libjansson4

RUN mkdir -p /nexus/bin
VOLUME /nexus/db
COPY --from=builder /nexus/lib /nexus/lib

FROM runtime AS nexus-createdb
COPY --from=builder /nexus/bin/nexus-createdb /nexus/bin

FROM runtime AS nexus-migratedb
COPY --from=builder /nexus/bin/nexus-migratedb /nexus/bin
CMD [ "/nexus/bin/nexus-migratedb" ]

FROM runtime AS nexus-builder
COPY --from=builder /nexus/bin/nexus-builder /nexus/bin
CMD [ "/nexus/bin/nexus-builder", "/nexus/db" ]
