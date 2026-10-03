#ifndef LLVM_TRANSFORMS_UTILS_STRENGTHREDUCTION_H
#define LLVM_TRANSFORMS_UTILS_STRENGTHREDUCTION_H

#include "llvm/IR/PassManager.h"      // <-- defines PassInfoMixin, must be here

namespace llvm {

class StrengthReductionPass
    : public OptionalPassInfoMixin<StrengthReductionPass> {
public:
  LLVM_ABI PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif
