#include "llvm/Transforms/Utils/StrengthReduction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

PreservedAnalyses
StrengthReductionPass::run(Function &F, FunctionAnalysisManager &AM) {
  std::vector<Instruction *> ToErase;

  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      auto *BO = dyn_cast<BinaryOperator>(&I);
      if (!BO || BO->getOpcode() != Instruction::Mul)
        continue;                        // only interested in 'mul'

      Value *op0 = BO->getOperand(0);
      Value *op1 = BO->getOperand(1);

      // mul is commutative: the constant may be on either side
      ConstantInt *CI = dyn_cast<ConstantInt>(op1);
      Value *Other = op0;
      if (!CI) {                         // constant wasn't on the right...
        CI = dyn_cast<ConstantInt>(op0); // ...try the left
        Other = op1;
      }
      if (!CI)
        continue;                        // no constant operand at all

      const APInt &Val = CI->getValue();
      if (!Val.isPowerOf2())
        continue;                        // only powers of two: 2,4,8,16...

      unsigned shiftAmt = Val.exactLogBase2();   // 8 -> 3, 2 -> 1

      errs() << "Strength reduction applied to: " << I
             << "   (shift left by " << shiftAmt << ")\n";

      // build   Other << shiftAmt   and insert it right before the mul
      IRBuilder<> Builder(BO);
      Value *Shl = Builder.CreateShl(
          Other, ConstantInt::get(BO->getType(), shiftAmt));

      BO->replaceAllUsesWith(Shl);       // make everyone use the shift
      ToErase.push_back(BO);             // mark the old mul for removal
    }
  }

  for (Instruction *I : ToErase)
    I->eraseFromParent();                // delete the muls we replaced

  return ToErase.empty() ? PreservedAnalyses::all()
                         : PreservedAnalyses::none();
}