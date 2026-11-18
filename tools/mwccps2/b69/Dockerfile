# Builds the native mwccps2 build 69 (Silent Hill 3's compiler).
#
#   docker build -f tools/mwccps2/b69/Dockerfile -t mwccps2-b69 .
#   docker run --rm -v "$PWD/build:/out" mwccps2-b69
FROM ubuntu:24.04

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates clang lld python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
# the originals, for the glue's data: decomp.me's public images, checked by digest
COPY tools/mwccps2/b69/get_orig.py tools/mwccps2/b69/get_orig.py
RUN python3 tools/mwccps2/b69/get_orig.py && test -f orig/PS2_3_0_3_b69/mwccps2.exe

COPY include include
COPY src src
COPY tools/mwccpsp tools/mwccpsp
COPY tools/mwccps2/native_core.py tools/mwccps2/native_core.py
COPY tools/mwccps2/b69 tools/mwccps2/b69
ARG JOBS
RUN python3 tools/mwccps2/b69/native_b69.py -j "${JOBS:-$(nproc)}" -o /mwccps2-b69

CMD ["cp", "/mwccps2-b69", "/out/mwccps2-b69"]
