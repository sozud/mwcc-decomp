#!/usr/bin/env python3
"""Build the native mwccps2 2.3.3: build/PS2_2_3_3/native/<arch>/mwccps2 (see ../native_core.py).

native_b233.py [-j N] [--arch x86_64|arm64] [--only UNIT] [-o OUT]
"""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import native_core  # noqa: E402

if __name__ == "__main__":
    native_core.main("b233")
