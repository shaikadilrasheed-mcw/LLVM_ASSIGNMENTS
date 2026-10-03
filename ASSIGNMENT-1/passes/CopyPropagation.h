#ifndef LLVM_TRANSFORMS_UTILS_COPYPROPAGATION_H     // ← unique guard!
#define LLVM_TRANSFORMS_UTILS_COPYPROPAGATION_H
#include "llvm/IR/PassManager.h"
namespace llvm {
class CopyPropagationPass : public OptionalPassInfoMixin<CopyPropagationPass> {
public:
  LLVM_ABI PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} // namespace llvm
#endif // LLVM_TRANSFORMS_UTILS_COPYPROPAGATION_H