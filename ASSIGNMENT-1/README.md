# LLVM Assignments

Custom LLVM optimization passes written for the **new pass manager**, built into
`opt`. Each pass transforms LLVM IR to demonstrate a classic compiler optimization.

## Assignment 1 — Optimization Passes

Five optimization passes, with their test C programs and the before/after IR they
produce.

---

## Repository layout

```
LLVM_ASSIGNMENTS/
└── ASSIGNMENT-1/
    ├── README.md    # this file
    ├── passes/      # the pass sources (.cpp + .h) and registration notes
    └── tests/       # the .c test cases and their *_unopt.ll / *_opt.ll IR
```

> The `clang` / `opt` commands below are run from the LLVM **build** directory and
> use `../test/...` paths relative to it — they are independent of this repository's
> folder layout. The repo just stores the pass sources and test files.

---

## Build setup (one time)

These passes live in `llvm/lib/Transforms/Utils` and are registered with the new
pass manager. To build them into `opt`:

1. Copy each `*.cpp` into `llvm/lib/Transforms/Utils/`
   and each `*.h` into `llvm/include/llvm/Transforms/Utils/`.
2. Add each `.cpp` to the `LLVMTransformUtils` source list in
   `llvm/lib/Transforms/Utils/CMakeLists.txt`.
3. Register each pass in `llvm/lib/Passes/PassRegistry.def` (FUNCTION_PASS section):
   ```cpp
   FUNCTION_PASS("strength-reduction", StrengthReductionPass())
   FUNCTION_PASS("algebraic-identity", AlgebraicIdentityPass())
   FUNCTION_PASS("copy-propagation",   CopyPropagationPass())
   FUNCTION_PASS("constant-folding2",  ConstantFolding2Pass())
   FUNCTION_PASS("custom-dce",         CustomDCEPass())
   ```
4. Add the matching `#include` for each header in `llvm/lib/Passes/PassBuilder.cpp`.
5. Build:
   ```
   ninja opt
   ```

### Common flags used below

- `clang -S -emit-llvm` — emit human-readable LLVM IR (`.ll`).
- `-Xclang -disable-O0-optnone` — strip the `optnone` attribute so `opt` is allowed
  to run passes on the unoptimized IR.
- `opt -S -passes=<name>` — run the named pass and print textual IR.
- `mem2reg` is run first for several passes: it promotes stack slots
  (`alloca`/`load`/`store`) into SSA registers, which exposes the patterns the pass
  needs (constants, same-value operands, dead values).

---

## 1. Strength Reduction

**What it does:** replaces an expensive multiply by a power of two with a cheap
left shift — `x * 2^n` becomes `x << n` (e.g. `x * 8` → `x << 3`). It checks each
`mul` instruction, finds a constant operand on either side (multiplication is
commutative), confirms the constant is a power of two, computes the shift amount
with `exactLogBase2()`, builds the `shl`, redirects all uses to it, and deletes the
old `mul`.

**Build & run:**
```
clang -S -emit-llvm -Xclang -disable-O0-optnone ../test/SR_2.c -o ../test/SR_2_unopt.ll
.\bin\opt.exe -S -passes=strength-reduction ../test/SR_2_unopt.ll -o ../test/SR_2_opt.ll
```

**Result:** `mul` by a power of two is rewritten as `shl`. No `mem2reg` needed —
the pattern exists in the raw IR.

---

## 2. Algebraic Identity

**What it does:** simplifies operations that are always trivial by algebra,
regardless of input values:
- `x * 1` → `x`
- `x + 0` → `x`
- `x - x` → `0`
- `x / x` → `1`

For each binary operator it checks the opcode and either the constant operand
(`isOne()` / `isZero()`) or whether both operands are the same value, then replaces
the instruction with the other operand or a constant.

**Build & run:**
```
clang -S -emit-llvm -Xclang -disable-O0-optnone ..\test\Algebraic_1.c -o ..\test\Algebraic_unopt.ll
.\bin\opt.exe -S -passes="mem2reg,algebraic-identity" ..\test\Algebraic_unopt.ll -o ..\test\Algebraic_opt.ll
```

**Why `mem2reg` first:** in raw IR, `x/x` and `x-x` are two *different* load
instructions even though they read the same variable. `mem2reg` promotes them to a
single SSA value, so "same operand" becomes detectable.

---

## 3. Copy Propagation

**What it does:** when a variable is a plain copy of another (`c = d`), later uses
of `c` are made to use `d` directly. The pass tracks a map of
`location -> the location it is a copy of`. It detects a copy as a `store` whose
stored value is a `load` from another location, then repoints later loads of the
copy to load from the source instead. It invalidates entries whenever either
variable is overwritten, so a stale value is never propagated.

**Build & run:**
```
clang -S -emit-llvm -Xclang -disable-O0-optnone ../test/ConstantProp2.c -o ../test/ConstantProp2_unopt.ll
.\bin\opt.exe -S -passes=copy-propagation ../test/ConstantProp2_unopt.ll -o ../test/ConstantProp2_opt.ll
```

**Result:** `load %c` becomes `load %d`; the now-unused copy becomes dead and can be
removed by DCE. Run *without* `mem2reg` — it already forwards copies during SSA
construction, which would leave nothing to propagate.

---

## 4. Constant Folding

**What it does:** evaluates operations whose operands are both constants at compile
time (e.g. `4 + 2` → `6`), replacing the instruction with the computed constant.
Processing instructions top to bottom with `replaceAllUsesWith` lets results
cascade (`add 4,2 → 6`, then `add 6,3 → 9`). It uses `APInt` arithmetic for
`add`, `sub`, `mul`, `sdiv`, `udiv`, `srem`, and guards against divide-by-zero.

**Build & run:**
```
clang -S -emit-llvm -Xclang -disable-O0-optnone ../test/ConstantFolding_1.c -o ../test/ConstantFolding_1_unopt.ll
.\bin\opt.exe -S -passes="mem2reg,constant-folding2" ../test/ConstantFolding_1_unopt.ll -o ../test/ConstantFolding_1_opt.ll
```

**Why `mem2reg` first:** it does the constant *propagation* — turning variables like
`a` and `b` into their literal constant values — so the operands become constants
that this pass can then fold.

---

## 5. Dead Code Elimination (DCE)

**What it does:** removes instructions whose result is never used and which have no
side effects. An instruction is deleted when `use_empty()` is true,
`mayHaveSideEffects()` is false, and it is not a terminator. The pass loops to a
fixpoint, so deleting one instruction can expose newly-dead operands that are then
also removed.

**Build & run:**
```
clang -S -emit-llvm -Xclang -disable-O0-optnone ../test/DCE_2.c -o ../test/DCE_2_unopt.ll
.\bin\opt.exe -S -passes="mem2reg,custom-dce" ../test/DCE_2_unopt.ll -o ../test/DCE_2_opt.ll
```

**Why `mem2reg` first:** it removes redundant *stores* (dead-store elimination,
which pure DCE can't touch because stores have side effects) and turns unused
computations into instructions with no uses, which this pass then deletes.

---

## Capturing a pass's debug output

Each pass prints what it changed to stderr. To save that to a file (PowerShell):
```
.\bin\opt.exe -S -passes="mem2reg,constant-folding2" ../test/ConstantFolding_1_unopt.ll -o ../test/ConstantFolding_1_opt.ll 2> ../test/ConstantFolding_1_log.txt
```

---

## Summary

| Pass | Transformation | Needs mem2reg |
|------|----------------|:-------------:|
| strength-reduction | `x * 2^n` → `x << n` | No |
| algebraic-identity | `x*1`, `x+0`, `x-x`, `x/x` → simplified | Yes |
| copy-propagation | `c = d; use c` → use `d` | No |
| constant-folding2 | fold constant expressions | Yes |
| custom-dce | remove unused, side-effect-free instructions | Yes |