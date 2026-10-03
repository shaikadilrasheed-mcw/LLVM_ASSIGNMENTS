#ifndef LLVM_TRANSFORMS_UTILS_CONSTANTFOLDING2_H    // ← unique guard!
#define LLVM_TRANSFORMS_UTILS_CONSTANTFOLDING2_H
#include "llvm/IR/PassManager.h"
namespace llvm {
class ConstantFolding2Pass : public OptionalPassInfoMixin<ConstantFolding2Pass> {
public:
  LLVM_ABI PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} // namespace llvm
#endif // LLVM_TRANSFORMS_UTILS_CONSTANTFOLDING2_H