#include "llvm/Transforms/Utils/CopyPropagation.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include <map>

using namespace llvm;

PreservedAnalyses
CopyPropagationPass::run(Function &F, FunctionAnalysisManager &AM) {
  bool changed = false;

  for (BasicBlock &BB : F) {
    // copyMap[dst] = src  means: location dst currently holds a copy of src
    std::map<Value *, Value *> copyMap;

    for (Instruction &I : BB) {

      if (auto *St = dyn_cast<StoreInst>(&I)) {
        Value *dst = St->getPointerOperand();   // location being written
        Value *val = St->getValueOperand();     // value being written

        // This store changes 'dst', so prior facts about it are now stale:
        copyMap.erase(dst);                       // dst is no longer a known copy
        for (auto it = copyMap.begin(); it != copyMap.end(); ) {
          if (it->second == dst) it = copyMap.erase(it);  // copies OF dst are stale
          else ++it;
        }

        // Is this a copy?   store (load %src), %dst   <=>   dst = src
        if (auto *L = dyn_cast<LoadInst>(val)) {
          Value *src = L->getPointerOperand();
          if (src != dst)
            copyMap[dst] = src;                    // record: dst is a copy of src
        }
      }

      else if (auto *Ld = dyn_cast<LoadInst>(&I)) {
        Value *ptr = Ld->getPointerOperand();
        auto it = copyMap.find(ptr);
        if (it != copyMap.end()) {
          errs() << "Copy propagation applied: " << I
                 << "   (now loads from the source variable)\n";
          Ld->setOperand(0, it->second);          // load %dst  ->  load %src
          changed = true;
        }
      }
    }
  }

  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}