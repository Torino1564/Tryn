#include "Compiler.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include <commdlg.h>

#include "Core/src/log/Log.h"
#include <TrynEditor/third/TinyProcessLib/process.hpp>
#include <filesystem>
#include <Core/src/utl/String.h>
using namespace std::string_literals;

namespace tryn::ed
{
	void Compiler::CompileToDLL(const std::filesystem::path& sourceFile, const bool debug)
	{
		const auto currentWD = std::filesystem::current_path();
        auto cmdProcess = TinyProcessLib::Process(
            LR"(C:\Windows\System32\cmd.exe)", {},  // Launch cmd.exe
        [](const char *output, size_t n) { std::cout << std::string(output, n); },  // Capture stdout
        [](const char *error, size_t n) { std::cerr << std::string(error, n); },   // Capture stderr
        true);

		cmdProcess.write( "\"" + std::string(R"(C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat)") + "\"");
		cmdProcess.write("\n");
		Sleep(100);

		if (debug)
		{
			auto pdbPath = sourceFile; pdbPath.replace_extension("pdb");
			cmdProcess.write(std::format("del {}", pdbPath.string()));
			cmdProcess.write("\n");
		}
		const auto includeDir = std::filesystem::current_path().parent_path();

		cmdProcess.write(std::format("cd {}", sourceFile.parent_path().string()));
		cmdProcess.write("\n");

		cmdProcess.write(std::format("cl.exe /std:c++latest /EHsc {} /LD /I {} \"{}\" /link  /DLL {}", debug ? "/Od /Zi /MDd" : "/O2 /MD", includeDir.string(), sourceFile.string(), debug ? "/DEBUG " : ""));
		cmdProcess.write("\n");
		Sleep(100);

		cmdProcess.write("exit\n");
        cmdProcess.get_exit_status();
		SetCurrentDirectoryA(currentWD.string().c_str());
	}
}
