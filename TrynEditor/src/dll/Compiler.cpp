#include "Compiler.h"
#include <iostream>
#include <vector>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <llvm/Support/FileSystem.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <llvm/TargetParser/Host.h>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

namespace tryn::ed
{
	void Compiler::CompileToDLL(const std::string& sourceFile, const std::string& objectFile)
	{
        clang::CompilerInstance compiler;

        // Setup diagnostics
        clang::IntrusiveRefCntPtr<clang::DiagnosticOptions> diagOpts = new clang::DiagnosticOptions();
        clang::TextDiagnosticPrinter diagPrinter(llvm::errs(), diagOpts.get());
        clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diagnostics(
            new clang::DiagnosticsEngine(new clang::DiagnosticIDs(), diagOpts, &diagPrinter));

        compiler.createDiagnostics(&diagPrinter, false);

        // Set target options (triple ensures correct ABI)
        compiler.getTargetOpts().Triple = llvm::sys::getDefaultTargetTriple();
        compiler.createTarget();

        // Configure compiler invocation arguments
        std::vector<const char*> args = {
            sourceFile.c_str(),
            "-c",         // Compile to object file
            "-O2",        // Optimization level 2
            "-fPIC",      // Position-independent code (for shared libraries)
            "-o", objectFile.c_str()
        };
        std::shared_ptr<clang::CompilerInvocation> pInv = std::make_shared< clang::CompilerInvocation>();
        const auto result1= CompilerInvocation::CreateFromArgs(*pInv, args, *diagnostics);

        compiler.setInvocation(pInv);
        clang::EmitObjAction action;

        const auto result2 = compiler.ExecuteAction(action);
	}
}
