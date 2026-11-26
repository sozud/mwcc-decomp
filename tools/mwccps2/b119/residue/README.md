# b119 memory-operand flags residue

b119's func_004C3C60 (0x4CF170) builds the MOperand for a memory VOperand
(kind 2/4) and never writes its rclass byte or flags word (`[esp+1..3]` on the
kind-4 path at 0x4CF18F). copypropagatestouse (0x553EF0) tests `flags & 8` on
every operand of a use, so whether a copy gets propagated depends on that stale
stack data.

- `resid.py OUT -- wibo mwccps2.exe ARGS`: logs every kind-4 operand's residue
  (rclass, flags, esp, caller). Case 7b7f568ef0c2 gives 0x0020, 0x6c3a,
  0x6c38, 0, 0, 0, 0x0046. Bit 3 of the flags word is bit 19 of a stale wibo
  heap pointer (0x6c3a44a8...).
- `wp.py 4cf18f N ADDR -- ...`: a hardware write watchpoint on the residue
  byte, stopping at the Nth hit. The writers are unrelated earlier calls
  (0x505c81, 0x57ca73, 0x5607b1) that push heap pointers. Stack addresses
  shift with the environment, so take ADDR from resid.py run under the same
  environment.
- Measured in the sotn-decomp tree's container: native b119 with the constant at 8
  gets 855/1118 and with 0 gets 847/1118. 78 cases pass only with 0 and 86
  only with 8, so at least 164 cases (15%) are decided by the residue. The
  union is 933. The remaining 185 fail with both constants; some of those
  mix residues within one function.
- `replay.py CASES NATIVE OUT` replays the original's residue into the native;
  of the 238 native failures only 16 still failed, and after the 2026-11-15
  ports (tools/mwccps2/README.md, b119 section) 4 do, and after
  CClass_OptimizeBitFieldAccess and the asm propagation hook only
  1419d6e2d123, whose residue depends on the environment size (the original
  fails it in replay.py's environment too; replaying words recorded where it
  passes, the native passes). RESID_CACHE=DIR keeps and reuses the recorded
  words, so a rerun only runs the native.

The residue is address-dependent UB, like b69's hascall. A 64-bit native
cannot reproduce wibo's heap addresses (its structures have other sizes), so
the native b119 ceiling stays well below the original. Use the original
under wibo to match.
