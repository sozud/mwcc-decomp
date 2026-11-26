#!/usr/bin/env python3
"""Build the native mwccps2 3.0 011126 (Persona 3 FES): build/PS2_3_0_011126/native/<arch>/mwccps2 (see ../native_core.py).

native_b011126.py [-j N] [--arch x86_64|arm64] [--only UNIT] [-o OUT]
"""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import native_core  # noqa: E402

if __name__ == "__main__":
    native_core.main("b011126")
