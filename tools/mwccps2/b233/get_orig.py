#!/usr/bin/env python3
"""Download the original PS2 compiler 2.3.3 (mwccps2.exe, 2000-09-06: Yu-Gi-Oh! Duelists of the Roses)
into orig/PS2_2_3_3/.

    get_orig.py [-o DIR]

decomp.me publishes its compilers as public container images
(github.com/decompme/compilers, ghcr.io/decompme/compilers/ps2/
mwcps2-2.3.3-000906). The image has a single layer, a tarball holding
compilers/ps2/mwcps2-2.3.3-000906/; this fetches that layer by its digest
(content-addressed, so the image tag moving doesn't change what is fetched),
checks it, and takes mwccps2.exe and the license manager DLL it loads.
"""

import argparse
import hashlib
import io
import json
import os
import tarfile
import urllib.request

REGISTRY = "https://ghcr.io"
IMAGE = "decompme/compilers/ps2/mwcps2-2.3.3-000906"
LAYER = "sha256:759c4347f8277222b046b73c5260253fb61f0b18253ce35f6bac543aee71e95e"
PREFIX = "compilers/ps2/mwcps2-2.3.3-000906/"
FILES = {
    "mwccps2.exe": "6375fd27814a1cb7eddbc229ecbd15b57d83562f95be23804f0a06ad4814e2df",
    "LMGR326B.DLL": "bd9415ab11f4aef42c372e8547a8303b02464b59549745be597cf09e460737e8",
}

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", "..", ".."))


def get(url, headers=None):
    with urllib.request.urlopen(urllib.request.Request(url, headers=headers or {}), timeout=60) as r:
        return r.read()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-o", "--out", default=os.path.join(REPO, "orig", "PS2_2_3_3"))
    a = ap.parse_args()
    if all(os.path.exists(os.path.join(a.out, f)) for f in FILES):
        have = {f: hashlib.sha256(open(os.path.join(a.out, f), "rb").read()).hexdigest() for f in FILES}
        if have == FILES:
            print(f"{a.out}: already there")
            return
    token = json.loads(get(f"{REGISTRY}/token?scope=repository:{IMAGE}:pull"))["token"]
    blob = get(f"{REGISTRY}/v2/{IMAGE}/blobs/{LAYER}", {"Authorization": f"Bearer {token}"})
    if "sha256:" + hashlib.sha256(blob).hexdigest() != LAYER:
        raise SystemExit(f"{IMAGE}: the layer doesn't match {LAYER}")
    os.makedirs(a.out, exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(blob), mode="r:gz") as tar:
        for name, sha in FILES.items():
            data = tar.extractfile(PREFIX + name).read()
            if hashlib.sha256(data).hexdigest() != sha:
                raise SystemExit(f"{name}: SHA-256 mismatch")
            open(os.path.join(a.out, name), "wb").write(data)
    print(f"{a.out}: " + ", ".join(FILES))


if __name__ == "__main__":
    main()
