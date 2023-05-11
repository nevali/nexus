FROM debian:bullseye-slim AS builder

RUN apt-get update -qq && apt-get -q -y install make clang automake autoconf libtool pkg-config libjansson-dev libssl-dev

ENV CC="clang"
ENV CXX="clang++"
ENV CPPFLAGS="-g -O0 -W -Wall -Werror"

FROM builder AS nexus-build

RUN mkdir -p /src
WORKDIR /src
COPY . /src

RUN cd WARP && autoreconf -fvi && ./configure && make && make check
RUN make

FROM builder AS devcontainer

FROM debian:bullseye-slim AS nexus

RUN apt-get update -qq && apt-get install -qq -y libjansson4

RUN mkdir -p /nexus/bin

FROM nexus AS nexus-createdb
COPY --from=nexus-build /src/nexus-createdb /nexus/bin
CMD /nexus/bin/nexus-createdb

FROM nexus AS nexus-migratedb
VOLUME /nexus/db
COPY --from=nexus-build /src/nexus-migratedb /nexus/bin
CMD /nexus/bin/nexus-migratedb

FROM nexus AS nexus-builder
VOLUME /nexus/db
COPY --from=nexus-build /src/nexus-builder /nexus/bin
CMD [ "/nexus/bin/nexus-builder", "/nexus/db" ]

FROM nexus AS nexus-gdb
RUN apt-get install -q -y gdb
VOLUME /nexus/db
COPY --from=nexus-build /src/nexus-builder /nexus/bin
CMD [ "/usr/bin/gdb", "--args", "/nexus/bin/nexus-builder", "/nexus/db" ]
