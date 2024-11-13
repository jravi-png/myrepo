#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace llvm;

static cl::opt<std::string> InputFile("input", cl::desc("Input C++ file to optimize"), cl::positional);

class HeartRateOptimizer : public RecursiveASTVisitor<HeartRateOptimizer> {
public:
  using RecursiveASTVisitor<HeartRateOptimizer>::RecursiveASTVisitor;

  bool VisitBinaryOperator(BinaryOperator *BO) {
    if (BO->getOpcode() == BO_Div &&
        isa<DeclRefExpr>(BO->getLHS()) && isa<DeclRefExpr>(BO->getRHS())) {
      DeclRefExpr *LHS = cast<DeclRefExpr>(BO->getLHS());
      DeclRefExpr *RHS = cast<DeclRefExpr>(BO->getRHS());

      if (LHS->getDecl()->getName() == "sum" &&
          RHS->getDecl()->getName() == "totalSamples") {
        // Found the redundant division, remove it
        Sema &Sema = getContext().getSema();
        Expr *newRHS = LHS;
        Rewrite.Rewrite(RHS->getBeginLoc(), newRHS);
        return true;
      }
    }
    return false;
  }

private:
  ASTContext &getContext() { return getASTContext(); }
  Rewriter &Rewrite;
};

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, clang::tooling::CommonOptionsParser::WithASTConsumer);
  ClangTool Tool(OptionsParser.getCompilationDatabase(), OptionsParser.getSourcePathList());

  if (InputFile.empty()) {
    errs() << "Error: Please provide an input C++ file using -input=<file>\n";
    return 1;
  }

  std::unique_ptr<ASTConsumer> Consumer = std::make_unique<HeartRateOptimizer>(Tool.getRewriter());
  Tool.run(CompilationUnitBuilder(Consumer.get()));

  // Write the modified code back to the file
  Tool.getRewriter().getEditBuffer(InputFile).write();
  return 0;
}