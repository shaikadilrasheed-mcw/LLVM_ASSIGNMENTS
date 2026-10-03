#include "llvm/Transforms/Utils/ConstantFolding2.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

PreservedAnalyses
ConstantFolding2Pass::run(Function &F, FunctionAnalysisManager &AM) {
  std::vector<Instruction *> ToErase;

  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      auto *BO = dyn_cast<BinaryOperator>(&I);
      if (!BO)
        continue;

      // both operands must be constant integers
      auto *C0 = dyn_cast<ConstantInt>(BO->getOperand(0));
      auto *C1 = dyn_cast<ConstantInt>(BO->getOperand(1));
      if (!C0 || !C1)
        continue;

      const APInt &A = C0->getValue();
      const APInt &B = C1->getValue();
      APInt R;
      bool ok = true;

      switch (BO->getOpcode()) {
      case Instruction::Add:  R = A + B;        break;
      case Instruction::Sub:  R = A - B;        break;
      case Instruction::Mul:  R = A * B;        break;
      case Instruction::SDiv:
        if (B == 0) { ok = false; break; }      // guard divide-by-zero
        R = A.sdiv(B);                          // signed division
        break;
      case Instruction::UDiv:
        if (B == 0) { ok = false; break; }
        R = A.udiv(B);                          // unsigned division
        break;
      case Instruction::SRem:
        if (B == 0) { ok = false; break; }
        R = A.srem(B);
        break;
      default:
        ok = false;                             // opcode we don't fold
        break;
      }

      if (!ok)
        continue;

      Constant *folded = ConstantInt::get(BO->getType(), R);
      errs() << "Constant folding: " << I << "   =>   " << *folded << "\n";

      BO->replaceAllUsesWith(folded);           // use the computed constant
      ToErase.push_back(BO);                     // remove the old op
    }
  }

  for (Instruction *I : ToErase)
    I->eraseFromParent();

  return ToErase.empty() ? PreservedAnalyses::all()
                         : PreservedAnalyses::none();
}