# Builds the native mwccps2 3.0 011126 (Persona 3 FES's game-code compiler).
#
#   docker build -f tools/mwccps2/b011126/Dockerfile -t mwccps2-b011126 .
#   docker run --rm -v "$PWD/build:/out" mwccps2-b011126
FROM ubuntu:24.04

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates clang lld python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
# the originals, for the glue's data: decomp.me's public images, checked by digest
COPY tools/mwccps2/b011126/get_orig.py tools/mwccps2/b011126/get_orig.py
RUN python3 tools/mwccps2/b011126/get_orig.py && test -f orig/PS2_3_0_011126/mwccps2.exe \
    && test -f orig/PS2_3_0_1_b145/mwccps2.exe

COPY include include
COPY src src
COPY tools/mwccpsp tools/mwccpsp
COPY tools/mwccps2/native_core.py tools/mwccps2/native_core.py
COPY tools/mwccps2/b011126 tools/mwccps2/b011126
ARG JOBS
RUN python3 tools/mwccps2/b011126/native_b011126.py -j "${JOBS:-$(nproc)}" -o /mwccps2-b011126

CMD ["cp", "/mwccps2-b011126", "/out/mwccps2-b011126"]
