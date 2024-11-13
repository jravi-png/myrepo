#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/RefactoringTool.h"
#include "llvm/Support/CommandLine.h"

using namespace clang::tooling;
using namespace clang::ast_matchers;

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, "Remove redundant casts");
  ClangTool Tool(OptionsParser.getCompilerInvocation(),
                 OptionsParser.getSourcePathList());

  // Define the matcher to find redundant casts
  auto RedundantCastMatcher =
      castExpr(expr(declRefExpr(to(classDecl()))).bind("derived"),
               expr(declRefExpr(to(classDecl()))).bind("derived"));

  // Perform the refactoring using the matcher
  return Tool.run(RecursiveASTVisitorAction<>(RedundantCastMatcher,
                                              /*RemoveMatch=*/true));
}