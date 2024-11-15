#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace llvm;

class MathExpressionOptimizer : public RecursiveASTVisitor<MathExpressionOptimizer> {
public:
  explicit MathExpressionOptimizer(ASTContext &Context) : Context(Context) {}

  bool VisitBinaryOperator(BinaryOperator *Op) {
    // Optimize constant arithmetic operations
    Expr *LHS = Op->getLHS();
    Expr *RHS = Op->getRHS();

    if (const ConstantExpr *LHSConst = dyn_cast<ConstantExpr>(LHS),
        const ConstantExpr *RHSConst = dyn_cast<ConstantExpr>(RHS)) {
      const APValue &LHSVal = LHSConst->getValueInContext(Context);
      const APValue &RHSVal = RHSConst->getValueInContext(Context);

      APValue Result;
      switch (Op->getOpcode()) {
        case BO_Add:
          Result = LHSVal.getAddValue(RHSVal);
          break;
        case BO_Mul:
          Result = LHSVal.getMulValue(RHSVal);
          break;
        // Add more cases for other optimized operations
        default:
          return false; // Not an optimized operation
      }

      // Create a new ConstantExpr with the optimized result
      ConstantExpr *OptimizedExpr =
          ConstantExpr::get(Context.getFloatType(), Result);

      // Replace the original BinaryOperator with the optimized ConstantExpr
      ReplaceStmt(Op, OptimizedExpr);
      return false; // Stop visiting this subtree since we've made a change
    }

    return true; // Recursively visit children
  }

private:
  ASTContext &Context;
};

class ASTConsumerForOptimization : public ASTConsumer {
public:
  explicit ASTConsumerForOptimization(ASTContext &Context)
      : Optimizer(Context) {}

  void HandleTopLevelDecl(Decl *D) override {
    Optimizer.TraverseDecl(D);
  }

private:
  MathExpressionOptimizer Optimizer;
};

int main(int argc, const char **argv) {
  CommonOptionsParser Op(argc, argv, "ast-optimization");
  ClangTool Tool(Op.getCompilerInvocation());

  // Create the ASTConsumer
  ASTContext &Context = Tool.getASTContext();
  ASTConsumerForOptimization Consumer(Context);

  // Attach the ASTConsumer to the CompilerInstance
  Tool.attachASTConsumer(&Consumer);

  // Run the tool on the input file
  return Tool.run(newFrontendActionFactory<clang::FrontendAction>());
}