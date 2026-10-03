#include "llvm/Transforms/Utils/CustomDCE.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

PreservedAnalyses
CustomDCEPass::run(Function &F, FunctionAnalysisManager &AM) {
  bool everChanged = false;
  bool changed = true;

  while (changed) {                 // repeat until no more dead code (fixpoint)
    changed = false;
    std::vector<Instruction *> ToErase;

    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        // Dead = result unused, no side effects, and not a terminator.
        if (I.use_empty() &&
            !I.isTerminator() &&
            !I.mayHaveSideEffects()) {
          errs() << "Dead code eliminated: " << I << "\n";
          ToErase.push_back(&I);
        }
      }
    }

    for (Instruction *I : ToErase) {
      I->eraseFromParent();          // delete it
      changed = true;                // something changed → loop again
      everChanged = true;
    }
  }

  return everChanged ? PreservedAnalyses::none() : PreservedAnalyses::all();
}