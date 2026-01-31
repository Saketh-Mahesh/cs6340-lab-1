/*
 * Copyright Â© 2021 Georgia Institute of Technology (Georgia Tech). All Rights
 * Reserved. Template code for CS 6340 Software Analysis Instructors: Mayur Naik
 * and Chris Poch Head TAs: Kelly Parks and Joel Cooper
 *
 * Georgia Tech asserts copyright ownership of this template and all derivative
 * works, including solutions to the projects assigned in this course. Students
 * and other users of this template code are advised not to share it with others
 * or to make it available on publicly viewable websites including repositories
 * such as GitHub and GitLab. This copyright statement should not be removed
 * or edited. Removing it will be considered an academic integrity issue.
 *
 * We do grant permission to share solutions privately with non-students such
 * as potential employers as long as this header remains in full. However,
 * sharing with other current or future students or using a medium to share
 * where the code is widely available on the internet is prohibited and
 * subject to being investigated as a GT honor code violation.
 * Please respect the intellectual ownership of the course materials
 * (including exam keys, project requirements, etc.) and do not distribute them
 * to anyone not enrolled in the class. Use of any previous semester course
 * materials, such as tests, quizzes, homework, projects, videos, and any other
 * coursework, is prohibited in this course. */
#include "Instrument.h"

using namespace llvm;

namespace instrument {

static const char *SanitizerFunctionName = "__dbz_sanitizer__";
static const char *CoverageFunctionName = "__coverage__";

FunctionType *getSanitizerFunctionType(LLVMContext &Context, unsigned numArgs) {
  Type *RetType = Type::getVoidTy(Context);
  std::vector<Type *> ArgTypes(numArgs, Type::getInt32Ty(Context));
  return FunctionType::get(RetType, ArgTypes, false);
}

/*
 * Implement divide-by-zero sanitizer.
 */
void instrumentSanitizer(Module *M, Function &F, Instruction &I) {
  LLVMContext &Context = M->getContext();
  FunctionType *FuncType = getSanitizerFunctionType(Context, 3);
  auto CalleeFunc = M->getOrInsertFunction("__dbz_sanitizer__", FuncType);

  Value *divisor = I.getOperand(1);
  DebugLoc loc = I.getDebugLoc();
  if (loc) {
    unsigned raw_line = loc.getLine();
    unsigned raw_col = loc.getCol();

    IRBuilder<> Builder(&I);
    llvm::Value *line = Builder.getInt32(raw_line);
    llvm::Value *col = Builder.getInt32(raw_col);

    std::vector<Value *> Args;
    Args.push_back(divisor);
    Args.push_back(line);
    Args.push_back(col);


    Builder.CreateCall(CalleeFunc, Args);
  }
}

/*
 * Implement code coverage instrumentation.
 */
void instrumentCoverage(Module *M, Function &F, Instruction &I) {
  LLVMContext &Context = M->getContext();
  FunctionType *FuncType = getSanitizerFunctionType(Context, 2);
  auto CalleeFunc = M->getOrInsertFunction("__coverage__", FuncType);

  DebugLoc loc = I.getDebugLoc();
  if (loc) {
    unsigned raw_line = loc.getLine();
    unsigned raw_col = loc.getCol();

    IRBuilder<> Builder(&I);
    llvm::Value *line = Builder.getInt32(raw_line);
    llvm::Value *col = Builder.getInt32(raw_col);

    std::vector<Value *> Args;
    Args.push_back(line);
    Args.push_back(col);


    Builder.CreateCall(CalleeFunc, Args);
  }
}

bool Instrument::runOnFunction(Function &F) {
  LLVMContext &Context = F.getContext();
  Module *M = F.getParent();

  std::vector<Instruction *> div_instructions;
  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      instrumentCoverage(M, F, I);
      if (I.getOpcode() == Instruction::SDiv ||
          I.getOpcode() == Instruction::UDiv) {
        div_instructions.push_back(&I);
      }
    }
  }

  for (Instruction *I : div_instructions) {
    instrumentSanitizer(M, F, *I);
  }

  return true;
}

char Instrument::ID = 1;
static RegisterPass<Instrument>
    X("Instrument", "Instrumentations for Dynamic Analysis", false, false);

} // namespace instrument
