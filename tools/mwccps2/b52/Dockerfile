# Builds the native mwccps2 3.0 build 52 (Monster Hunter's compiler).
#
#   docker build -f tools/mwccps2/b52/Dockerfile -t mwccps2-b52 .
#   docker run --rm -v "$PWD/build:/out" mwccps2-b52
FROM ubuntu:24.04

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates clang lld python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
# the originals, for the glue's data: decomp.me's public images, checked by digest
COPY tools/mwccps2/b52/get_orig.py tools/mwccps2/b52/get_orig.py
RUN python3 tools/mwccps2/b52/get_orig.py && test -f orig/PS2_3_0_b52/mwccps2.exe

COPY include include
COPY src src
COPY tools/mwccpsp tools/mwccpsp
COPY tools/mwccps2/native_core.py tools/mwccps2/native_core.py
COPY tools/mwccps2/b52 tools/mwccps2/b52
ARG JOBS
RUN python3 tools/mwccps2/b52/native_b52.py -j "${JOBS:-$(nproc)}" -o /mwccps2-b52

CMD ["cp", "/mwccps2-b52", "/out/mwccps2-b52"]
