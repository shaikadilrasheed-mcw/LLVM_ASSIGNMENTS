#include "llvm/Transforms/Utils/AlgebraicIdentity.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

// Are these two operands the SAME runtime value?
//   - identical SSA value  (works after mem2reg:  %x / %x)
//   - two loads from the same pointer (works on raw IR:  load %a ; load %a)
static bool sameValue(Value *A, Value *B) {
  if (A == B)
    return true;
  auto *LA = dyn_cast<LoadInst>(A);
  auto *LB = dyn_cast<LoadInst>(B);
  if (LA && LB && LA->getPointerOperand() == LB->getPointerOperand())
    return true;                     // caveat: assumes no store in between
  return false;
}

PreservedAnalyses
AlgebraicIdentityPass::run(Function &F, FunctionAnalysisManager &AM) {
  std::vector<Instruction *> ToErase;

  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      auto *BO = dyn_cast<BinaryOperator>(&I);
      if (!BO)
        continue;

      Value *op0 = BO->getOperand(0);
      Value *op1 = BO->getOperand(1);
      auto *C0 = dyn_cast<ConstantInt>(op0);
      auto *C1 = dyn_cast<ConstantInt>(op1);
      Value *replacement = nullptr;

      switch (BO->getOpcode()) {

      case Instruction::Mul:                 // x * 1  ->  x
        if (C1 && C1->isOne())      replacement = op0;
        else if (C0 && C0->isOne()) replacement = op1;
        else if ((C1 && C1->isZero()) || (C0 && C0->isZero()))
          replacement = ConstantInt::get(BO->getType(), 0);   // x * 0 -> 0
        break;

      case Instruction::Add:                 // x + 0  ->  x
        if (C1 && C1->isZero())      replacement = op0;
        else if (C0 && C0->isZero()) replacement = op1;
        break;

      case Instruction::Sub:                 // x - x  ->  0 ,  x - 0 -> x
        if (sameValue(op0, op1))
          replacement = ConstantInt::get(BO->getType(), 0);
        else if (C1 && C1->isZero())
          replacement = op0;
        break;

      case Instruction::SDiv:                // x / x  ->  1
      case Instruction::UDiv:
        if (sameValue(op0, op1))
          replacement = ConstantInt::get(BO->getType(), 1);
        else if (C1 && C1->isOne())          // x / 1 -> x
          replacement = op0;
        break;

      default:
        break;
      }

      if (replacement) {
        errs() << "Algebraic identity simplified: " << I << "\n";
        BO->replaceAllUsesWith(replacement);
        ToErase.push_back(BO);
      }
    }
  }

  for (Instruction *I : ToErase)
    I->eraseFromParent();

  return ToErase.empty() ? PreservedAnalyses::all()
                         : PreservedAnalyses::none();
}