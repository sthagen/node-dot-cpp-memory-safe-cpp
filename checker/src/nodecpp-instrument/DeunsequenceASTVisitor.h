/* -------------------------------------------------------------------------------
* Copyright (c) 2019, OLogN Technologies AG
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the OLogN Technologies AG nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL OLogN Technologies AG BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* -------------------------------------------------------------------------------*/

#ifndef NODECPP_CHECKER_DEUNSEQUENCEASTVISITOR_H
#define NODECPP_CHECKER_DEUNSEQUENCEASTVISITOR_H

#include "SequenceCheck.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Format/Format.h"
#include "clang/Lex/Lexer.h"
#include "llvm/Support/raw_ostream.h"

namespace nodecpp {

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace std;


template<class T>
class RecursivePostOrderASTVisitor : public RecursiveASTVisitor<T> {
  public:
    /// Return whether this visitor should traverse post-order.
  bool shouldTraversePostOrder() const { return true; }
};

class ExpressionUnwrapperVisitor : public RecursivePostOrderASTVisitor<ExpressionUnwrapperVisitor>{

  using Base = RecursiveASTVisitor<ExpressionUnwrapperVisitor>;
  const ASTContext *Context = nullptr;
  
  string StmtText;
  Range StmtRange;
  SourceRange StmtSourceRange;
  string Buffer; //TODO change to ostream or similar
  int &Index;
  bool ExtraBraces = false;
  list<pair<string, Range>> Reps;
public:

  ExpressionUnwrapperVisitor(const ASTContext *Context, int &Index)
    :Context(Context), Index(Index) {}

  bool unwrapExpression(Stmt *St, Expr *E, bool ExtraBraces) {
    auto R = printExprAsWritten(St, Context);
    if(!R.first)
      return false;

    StmtText = R.second;
    StmtSourceRange = St->getSourceRange();
    StmtRange = calcRange(StmtSourceRange);
    this->ExtraBraces = ExtraBraces;

    this->TraverseStmt(E);
    return hasReplacement();
  }
  // mb: copy and paste from lib/AST/StmtPrinter.cpp
  static pair<bool, StringRef> printExprAsWritten(Stmt *St,
                                const ASTContext *Context) {
    if (!Context)
      return {false, ""};
    bool Invalid = false;
    StringRef Source = Lexer::getSourceText(
        CharSourceRange::getTokenRange(St->getSourceRange()),
        Context->getSourceManager(), Context->getLangOpts(), &Invalid);
    return {!Invalid, Source};
  }

  bool isContained(unsigned Offset, unsigned Length, const Range& RHS) {
    return RHS.getOffset() >= Offset &&
           (RHS.getOffset() + RHS.getLength()) <= (Offset + Length);
  }

  string subStmtWithReplaces(Range RangeInStmtText) {

    //here do magic to replace existing replacemnts
    string B = StmtText;
    unsigned Offset = RangeInStmtText.getOffset();
    unsigned Length =  RangeInStmtText.getLength();

    for(auto It = Reps.crbegin(); It != Reps.crend(); ++It) {

      if(isContained(Offset, Length, It->second)) {
        Length += It->first.size();
        Length -= It->second.getLength();
        B.replace(It->second.getOffset(), It->second.getLength(), It->first);
      }
    }

    return B.substr(Offset, Length);
  }

  void unwrap(Expr *E) {

    if(StmtText.empty())
      return;

    bool LValue = E->isLValue();
    E = E->IgnoreParenImpCasts();
    if(isa<DeclRefExpr>(E))
      return;
    else if(isa<IntegerLiteral>(E))
      return;
    else if(isa<clang::StringLiteral>(E))
      return;
    else if(isa<CXXNullPtrLiteralExpr>(E))
      return;
    else if(isa<FloatingLiteral>(E))
      return;

    Range RangeInStmtText = toTextRange(calcRange(E->getSourceRange()));
    string Name = generateName();

    if(LValue) 
      Buffer += "auto& ";
    else
      Buffer += "auto&& ";
    
    Buffer += Name;
    Buffer += " = ";
    Buffer += subStmtWithReplaces(RangeInStmtText);
    Buffer += "; ";

    addReplacement(move(Name), RangeInStmtText);
  }

  Range calcRange(const SourceRange &Sr) {

    auto& Sm = Context->getSourceManager();
    SourceLocation SpellingBegin = Sm.getSpellingLoc(Sr.getBegin());
    SourceLocation SpellingEnd = Sm.getSpellingLoc(Sr.getEnd());
    
    std::pair<FileID, unsigned> Start = Sm.getDecomposedLoc(SpellingBegin);
    std::pair<FileID, unsigned> End = Sm.getDecomposedLoc(SpellingEnd);
    
    if (Start.first != End.first) return Range();

    //SourceRange is always in token
    End.second += Lexer::MeasureTokenLength(SpellingEnd, Sm, Context->getLangOpts());

    // const FileEntry *Entry = Sm.getFileEntryForID(Start.first);
    // this->FilePath = Entry ? Entry->getName() : InvalidLocation;
    return Range(Start.second, End.second - Start.second);
  }

  Range toTextRange(Range SrcRange) {

    assert(SrcRange.getOffset() - StmtRange.getOffset() + SrcRange.getLength() <= StmtText.size());
    return Range(SrcRange.getOffset() - StmtRange.getOffset(), SrcRange.getLength());
  }

  void addReplacement(string Text, Range R) {

    auto It = Reps.begin();
    while(It != Reps.end()) {
      if(R.overlapsWith(It->second)) {
        assert(R.contains(It->second));
        It = Reps.erase(It);
      }
      else
        ++It;
    }

    auto Jt = Reps.begin();
    while(Jt != Reps.end()) {
      if(R.getOffset() < Jt->second.getOffset()) {
        Reps.insert(Jt, {Text, R});
        return;
      }
      else
        ++Jt;
    }
    Reps.push_back({Text, R});
  }


  bool hasReplacement() const {
    return !Buffer.empty();
  }

  // tooling::Replacement makeInsert(SourceLocation Where) {

  //   // auto Sr = CharSourceRange::getCharRange(Where, Where);
  //   // tooling::Replacement R(Context->getSourceManager(), Sr,
  //   //                                   Buffer);
  //   return tooling::Replacement{};
  // } 

  tooling::Replacement makeFix() {

    Buffer += subStmtWithReplaces(Range(0, StmtText.size()));
    if(ExtraBraces) {
      Buffer += " };";
      Buffer.insert(0, "{ ");
    }


    auto CharSr = CharSourceRange::getTokenRange(StmtSourceRange);
    return Replacement(Context->getSourceManager(), CharSr, Buffer);
  }

  string generateName() {
    string Name = "nodecpp_" + to_string(Index);
    ++Index;
    return Name;
  }

  // bool VisitDeclRefExpr(DeclRefExpr *Dre) {
    
  // }

  // bool VisitCXXThisExpr(CXXThisExpr *E) {
  // }

  bool VisitCallExpr(CallExpr *E) {


    if(E->getNumArgs() > 1) {
      for(auto Each : E->arguments()) {
        unwrap(Each);
      }
    }
    return true;
  }

  bool VisitBinaryOperator(BinaryOperator *E) {

    if(E->isAdditiveOp() || E->isMultiplicativeOp() ||
      E->isBitwiseOp() || E->isComparisonOp() ||
      E->isRelationalOp()) {
        unwrap(E->getLHS());
        unwrap(E->getRHS());
      }

    return true;
  }

  bool VisitUnaryOperator(UnaryOperator *E) {
    
    if(E->isPostfix()) {
      unwrap(E->getSubExpr());
    }
    return true;
  }

};




class Deunsequence2ASTVisitor
  : public BaseASTVisitor<Deunsequence2ASTVisitor> {

//    using Base = clang::RecursiveASTVisitor<Deunsequence2ASTVisitor>;
//  clang::ASTContext &Context;
//  DzHelper &DzData;
  int Index = 0;
  /// Fixes to apply, grouped by file path.
  StringMap<Replacements> FileReplacements;

  void addFix(const Replacement& Replacement) {
    llvm::Error Err = FileReplacements[Replacement.getFilePath()].add(Replacement);
    // FIXME: better error handling (at least, don't let other replacements be
    // applied).
    if (Err) {
      llvm::errs() << "Fix conflicts with existing fix! "
                    << llvm::toString(std::move(Err)) << "\n";
      assert(false && "Fix conflicts with existing fix!");
    }
  }

  bool needExtraBraces(Stmt *St) {

    auto SList = Context.getParents(*St);

    auto SIt = SList.begin();

    if (SIt == SList.end())
      return true;

    return SIt->get<CompoundStmt>() == nullptr;
  }

  bool unwrapExpression(Stmt* St, Expr* E) {

    ExpressionUnwrapperVisitor V(&Context, Index);
    if(V.unwrapExpression(St, E, needExtraBraces(St))) {
      addFix(V.makeFix());
    }
    
    return true;
  }

public:
  const auto& getReplacements() const { return FileReplacements; }

  explicit Deunsequence2ASTVisitor(ASTContext &Context):
    BaseASTVisitor<Deunsequence2ASTVisitor>(Context) {}


  bool TraverseStmt(Stmt *St) {

    if(Expr* E = dyn_cast_or_null<Expr>(St)) {
      return unwrapExpression(St, E);
    }
    else
      return Base::TraverseStmt(St);
  }

  bool TraverseDeclStmt(DeclStmt *St) {
    
    if(St->isSingleDecl()) {
      if(VarDecl* D = dyn_cast_or_null<VarDecl>(St->getSingleDecl())) {
        if(Expr *E = D->getInit()) {
          return unwrapExpression(St, E);
        }
      }
    }
    return Base::TraverseDeclStmt(St);
  }

//  need to traverse while body, but not condition expression
  // bool TraverseWhileStmt(WhileStmt *St) {
  //   unwrapExpression(St, St->getCond());
  //   return Base::TraverseWhileStmt(St);
  // }

//   bool TraverseStmt(Stmt *St) {
//     // For every root expr, sent it to check and don't traverse it here
//     if(!St)
//       return true;

//     if(Expr *E = dyn_cast<Expr>(St)) {
// //      checkUnsequencedDezombiefy(Context, E, true);
//       return true;
//     }
//     else
//       return Base::TraverseStmt(St);
//   }

  // bool VisitStmt(Stmt *St) {

  //   // if(Expr* E = dyn_cast_or_null<Expr>(St)) {
  //   //   unwrapExpression(E, E);
  //   //   return true;
  //   // }
  //   // else
  //     return clang::RecursiveASTVisitor<Deunsequence2ASTVisitor>::VisitStmt(St);
  // }
};

} // namespace nodecpp

#endif // NODECPP_CHECKER_DEUNSEQUENCEASTVISITOR_H

