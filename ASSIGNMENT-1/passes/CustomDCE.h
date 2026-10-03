#ifndef LLVM_TRANSFORMS_UTILS_CUSTOMDCE_H      // ← unique guard!
#define LLVM_TRANSFORMS_UTILS_CUSTOMDCE_H
#include "llvm/IR/PassManager.h"
namespace llvm {
class CustomDCEPass : public OptionalPassInfoMixin<CustomDCEPass> {
public:
  LLVM_ABI PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} // namespace llvm
#endif // LLVM_TRANSFORMS_UTILS_CUSTOMDCE_H