#ifndef LLVM_TRANSFORMS_UTILS_ALGEBRAICIDENTITY_H
#define LLVM_TRANSFORMS_UTILS_ALGEBRAICIDENTITY_H

#include "llvm/IR/PassManager.h"      // <-- defines PassInfoMixin, must be here

namespace llvm {

class AlgebraicIdentityPass
    : public OptionalPassInfoMixin<AlgebraicIdentityPass> {
public:
  LLVM_ABI PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif
