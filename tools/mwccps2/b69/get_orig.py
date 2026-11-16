#!/usr/bin/env python3
"""Download the original compilers the native b69 build reads data from:
mwccps2.exe build 69 (version string 3.0.3, 2002-07-16; Silent Hill 3's compiler)
into orig/PS2_3_0_3_b69/ and mwccps2.exe 3.0.1 build 145 (2005-02-09; the copied
back end's data tables are read from it) into orig/PS2_3_0_1_b145/.

    get_orig.py

As tools/mwccps2/get_orig.py: decomp.me publishes its compilers as public
container images (ghcr.io/decompme/compilers/ps2/...), each a single layer;
the layer is fetched by its digest, checked, and the files taken from it.
"""

import argparse
import hashlib
import io
import json
import os
import tarfile
import urllib.request

REGISTRY = "https://ghcr.io"
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", ".."))
COMPILERS = {
    "PS2_3_0_3_b69": {
        "image": "decompme/compilers/ps2/mwcps2-3.0.3-020716",
        "layer": "sha256:c67856bc06b676353a6c64591d8f6ec2cc841317f1fc824f587170c11738c4c3",
        "prefix": "compilers/ps2/mwcps2-3.0.3-020716/",
        "files": {
            "mwccps2.exe": "c894c769efbce7938f7c334ee408e6e3c1fae2da033e02cbdecfe9f88210f236",
        },
    },
    "PS2_3_0_1_b145": {
        "image": "decompme/compilers/ps2/mwcps2-3.0.1b145-050209",
        "layer": "sha256:5d4efb590797c2c7122d3dd286c377be11917ee892978fb5d5e2c61ee5f34ccb",
        "prefix": "compilers/ps2/mwcps2-3.0.1b145-050209/",
        "files": {
            "mwccps2.exe": "c42c6cc4428435b8e4fda0dcdc45208c60f0a25cf0c7bfa73824e1496e8e544b",
        },
    },
}


def get(url, headers=None):
    with urllib.request.urlopen(urllib.request.Request(url, headers=headers or {}), timeout=60) as r:
        return r.read()


def fetch(out, c):
    files = c["files"]
    if all(os.path.exists(os.path.join(out, f)) for f in files):
        have = {f: hashlib.sha256(open(os.path.join(out, f), "rb").read()).hexdigest() for f in files}
        if have == files:
            print(f"{out}: already there")
            return
    image, layer = c["image"], c["layer"]
    token = json.loads(get(f"{REGISTRY}/token?scope=repository:{image}:pull"))["token"]
    blob = get(f"{REGISTRY}/v2/{image}/blobs/{layer}", {"Authorization": f"Bearer {token}"})
    if "sha256:" + hashlib.sha256(blob).hexdigest() != layer:
        raise SystemExit(f"{image}: the layer doesn't match {layer}")
    os.makedirs(out, exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(blob), mode="r:gz") as tar:
        names = {m.name.lower(): m.name for m in tar.getmembers()}
        for name, sha in files.items():
            member = names.get((c["prefix"] + name).lower())
            if member is None:
                raise SystemExit(f"{image}: no {c['prefix'] + name} in the layer")
            data = tar.extractfile(member).read()
            if hashlib.sha256(data).hexdigest() != sha:
                raise SystemExit(f"{name}: SHA-256 mismatch")
            open(os.path.join(out, name), "wb").write(data)
    print(f"{out}: " + ", ".join(files))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-o", "--out", default=os.path.join(REPO, "orig"), help="parent directory (default: orig/)")
    a = ap.parse_args()
    for d, c in COMPILERS.items():
        fetch(os.path.join(a.out, d), c)


if __name__ == "__main__":
    main()
