// ****************************************************************************
//  compiler.cpp                                                  XL project
// ****************************************************************************
//
//   File Description:
//
//    Just-in-time (JIT) compiler for XL trees using LLVM as a back-end
//
//
//
//
//
//
//
//
// ****************************************************************************
// This document is released under the GNU General Public License, with the
// following clarification and exception.
//
// Linking this library statically or dynamically with other modules is making
// a combined work based on this library. Thus, the terms and conditions of the
// GNU General Public License cover the whole combination.
//
// As a special exception, the copyright holders of this library give you
// permission to link this library with independent modules to produce an
// executable, regardless of the license terms of these independent modules,
// and to copy and distribute the resulting executable under terms of your
// choice, provided that you also meet, for each linked independent module,
// the terms and conditions of the license of that module. An independent
// module is a module which is not derived from or based on this library.
// If you modify this library, you may extend this exception to your version
// of the library, but you are not obliged to do so. If you do not wish to
// do so, delete this exception statement from your version.
//
// See http://www.gnu.org/copyleft/gpl.html and Matthew 25:22 for details
//  (C) 1992-2010 Christophe de Dinechin <christophe@taodyne.com>
//  (C) 2010 Taodyne SAS
// ****************************************************************************

#include "compiler.h"
#include "compiler-unit.h"
#include "errors.h"

#include <recorder/recorder.h>
#include <iostream>
#include <sstream>
#include <cstdarg>


RECORDER(compiler, 16, "General information about the XL compiler");


XL_BEGIN
// ============================================================================
//
//    Compiler - Global information about the LLVM compiler
//
// ============================================================================
// The Compiler class is where we store all the global information that
// persists during the lifetime of the program: LLVM data structures,
// LLVM definitions for frequently used types, XL runtime functions, ...
//

using namespace llvm;

Compiler::Compiler(kstring moduleName, unsigned opts, int argc, char **argv)
// ----------------------------------------------------------------------------
//   Initialize the various types and global functions we may need
// ----------------------------------------------------------------------------
    : jit               (argc, argv),
      voidTy            (jit.VoidType()),
      booleanTy         (jit.IntegerType(1)),
      integerTy         (jit.IntegerType<int>()),
      integer8Ty        (jit.IntegerType(8)),
      integer16Ty       (jit.IntegerType(16)),
      integer32Ty       (jit.IntegerType(32)),
      integer64Ty       (jit.IntegerType(64)),
      integer128Ty      (jit.IntegerType(128)),
      unsignedTy        (jit.IntegerType<unsigned>()),
      ulongTy           (jit.IntegerType<ulong>()),
      ulonglongTy       (jit.IntegerType<ulonglong>()),
      realTy            (jit.FloatType(64)),
      real32Ty          (jit.FloatType(32)),
      real64Ty          (jit.FloatType(64)),
      characterTy       (jit.IntegerType<char>()),
      charPtrTy         (jit.PointerType(characterTy)),
      textTy            (jit.StructType({jit.OpaqueType("textrep")}, "text")),
      infoTy            (jit.OpaqueType("Info")),
      infoPtrTy         (jit.PointerType(infoTy)),

#define TREE    ulongTy, infoPtrTy
#define TREE1   TREE, treePtrTy
#define TREE2   TREE1, treePtrTy
      treeTy            (jit.StructType({TREE},                 "Tree")),
      treePtrTy         (jit.PointerType(treeTy)),
      treePtrPtrTy      (jit.PointerType(treePtrTy)),
      integerTreeTy     (jit.StructType({TREE, ulonglongTy},    "Integer")),
      integerTreePtrTy  (jit.PointerType(integerTreeTy)),
      realTreeTy        (jit.StructType({TREE, realTy},         "Real")),
      realTreePtrTy     (jit.PointerType(realTreeTy)),
      textTreeTy        (jit.StructType({TREE, textTy},         "Text")),
      textTreePtrTy     (jit.PointerType(textTreeTy)),
      nameTreeTy        (jit.StructType({TREE, textTy},         "Name")),
      nameTreePtrTy     (jit.PointerType(nameTreeTy)),
      blockTreeTy       (jit.StructType({TREE1},                "Block")),
      blockTreePtrTy    (jit.PointerType(blockTreeTy)),
      prefixTreeTy      (jit.StructType({TREE2},                "Prefix")),
      prefixTreePtrTy   (jit.PointerType(prefixTreeTy)),
      postfixTreeTy     (jit.StructType({TREE2},                "Postfix")),
      postfixTreePtrTy  (jit.PointerType(postfixTreeTy)),
      infixTreeTy       (jit.StructType({TREE2, textTy},        "Infix")),
      infixTreePtrTy    (jit.PointerType(infixTreeTy)),
      scopeTy           (jit.StructType({TREE2},                "Scope")),
      scopePtrTy        (prefixTreePtrTy),
#undef TREE
#undef TREE1
#undef TREE2

      evalTy            (jit.FunctionType(treePtrTy, {scopePtrTy, treePtrTy})),
      evalFnTy          (jit.PointerType(evalTy))
{
    record(compiler, "Created compiler %p", this);

    // Set optimization level
    jit.SetOptimizationLevel(opts);
}


Compiler::~Compiler()
// ----------------------------------------------------------------------------
//    Destructor deletes the various things we had created
// ----------------------------------------------------------------------------
{
    record(llvm_stats, "LLVM statistics");
    if (RECORDER_TRACE(llvm_stats))
        jit.PrintStatistics();
}


Tree * Compiler::Evaluate(Scope *scope, Tree *source)
// ----------------------------------------------------------------------------
//   Compile the tree, then run the evaluation function
// ----------------------------------------------------------------------------
//   This is the entry point used to compile a top-level XL program.
//   It will process all the declarations in the program and then compile
//   the rest of the code as a function taking no arguments.
{
    record(compiler, "Compiling program %t in scope %t", source, scope);
    if (!source || !scope)
        return NULL;

    CompilerUnit unit(*this, scope, source);
    eval_fn code = unit.Compile();
    record(compiler, "Compiled %t in scope %t as %p",
           source, scope, (void *) code);

    if (!code)
    {
        Ooops("Error compiling $1", source);
        return source;
    }

    Tree *result = code(scope, source);
    return result;
}


Tree * Compiler::TypeCheck(Scope *, Tree *type, Tree *val)
// ----------------------------------------------------------------------------
//   Compile a type check
// ----------------------------------------------------------------------------
{
    return val;
}


bool Compiler::TypeAnalysis(Scope *scope, Tree *program)
// ----------------------------------------------------------------------------
//   Perform type analysis on the input tree
// ----------------------------------------------------------------------------
{
    CompilerUnit unit(*this, scope, program);
    return unit.TypeAnalysis();
}


PointerType_p Compiler::TreeMachineType(Tree *tree)
// ----------------------------------------------------------------------------
//    Return the LLVM tree type associated to a given XL expression
// ----------------------------------------------------------------------------
{
    switch(tree->Kind())
    {
    case INTEGER:
        return integerTreePtrTy;
    case REAL:
        return realTreePtrTy;
    case TEXT:
        return textTreePtrTy;
    case NAME:
        return nameTreePtrTy;
    case INFIX:
        return infixTreePtrTy;
    case PREFIX:
        return prefixTreePtrTy;
    case POSTFIX:
        return postfixTreePtrTy;
    case BLOCK:
        return blockTreePtrTy;
    }
    assert(!"Invalid tree type");
    return treePtrTy;
}


XL_END