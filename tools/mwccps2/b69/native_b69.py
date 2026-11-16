#!/usr/bin/env python3
"""Build the native mwccps2 b69: build/PS2_3_0_3_b69/native/<arch>/mwccps2 (see ../native_core.py).

native_b69.py [-j N] [--arch x86_64|arm64] [--only UNIT] [-o OUT]
"""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import native_core  # noqa: E402

if __name__ == "__main__":
    native_core.main("b69")
