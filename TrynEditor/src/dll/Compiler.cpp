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

#include <Core/src/log/Log.h>

#include "Core/src/utl/String.h"

#include <thread>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

namespace tryn::ed
{
    class ClangDiagnosticsChannel
    {
    public:
        bool Run()
        {
            kernelThread_ = std::jthread(&ClangDiagnosticsChannel::KernelLoop, this);
            return true;
        }
        static ClangDiagnosticsChannel& Get()
        {
	        static ClangDiagnosticsChannel singleton;
            return singleton;
        }

        void KernelLoop()
        {
	        while (true)
		       {
			       if (llvm::errs().has_error())
                       trylog.warn(utl::ToWide(std::format("Clang diagnostics error: Code: {} - Message: {}\n", llvm::errs().error().value(), llvm::errs().error().message())));
		       }
        }

    private:
        std::jthread kernelThread_;

    };

	void Compiler::CompileToDLL(const std::string& sourceFile, const std::string& objectFile)
	{
        static auto diag = ClangDiagnosticsChannel::Get().Run();

        clang::CompilerInstance compiler;

        // Setup diagnostics
        clang::IntrusiveRefCntPtr<clang::DiagnosticOptions> diagOpts = new clang::DiagnosticOptions();

        diagOpts->DiagnosticLogFile = "diagFile.txt";

        clang::TextDiagnosticPrinter diagPrinter(llvm::errs(), diagOpts.get());
        clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diagnostics(
            new clang::DiagnosticsEngine(new clang::DiagnosticIDs(), diagOpts, &diagPrinter));

        compiler.createDiagnostics(&diagPrinter, false);

        // Set target options (triple ensures correct ABI)
        compiler.getTargetOpts().Triple = llvm::sys::getDefaultTargetTriple();
        compiler.createTarget();

        // Configure compiler invocation arguments
        std::vector<const char*> args = {
            "-c",         // Compile to object file
            sourceFile.c_str(),
            "std=c++23",
            "I",
            "C:\\dev\\tryn",
            "-O2",        // Optimization level 2
            "-o", objectFile.c_str(),
            "Test"
        };
        auto pInv = std::shared_ptr<CompilerInvocation>(newInvocation(diagnostics.get(), args, "test"));

        compiler.setInvocation(pInv);
        clang::EmitObjAction action;

        const auto result2 = compiler.ExecuteAction(action);
	}
}
