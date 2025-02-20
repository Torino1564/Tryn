#include "Compiler.h"
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

namespace tryn::ed
{
	void Compiler::CompileToDLL(const std::filesystem::path& path, std::string_view fileName)
	{
		   IntrusiveRefCntPtr<DiagnosticOptions> diagOpts = new DiagnosticOptions();
		TextDiagnosticPrinter *diagPrinter = new TextDiagnosticPrinter(llvm::errs(), diagOpts.get());
		IntrusiveRefCntPtr<DiagnosticsEngine> diags = new DiagnosticsEngine(
        IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()), diagOpts, diagPrinter);
	}
}
