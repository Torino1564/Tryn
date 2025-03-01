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
	void Compiler::CompileToDLL(const std::string& sourceFile)
	{
	//      OPENFILENAMEA fileName;
		
	//char filename[ MAX_PATH ];

		//OPENFILENAMEA ofn;
	//      ZeroMemory( &filename, sizeof( filename ) );
	//      ZeroMemory( &ofn,      sizeof( ofn ) );
	//      ofn.lStructSize  = sizeof( ofn );
	//      ofn.hwndOwner    = nullptr;  // If you have a window to center over, put its HANDLE here
	//      ofn.lpstrFilter  = "Executable Files\0*.txt\0Any File\0*.*\0";
	//      ofn.lpstrFile    = filename;
	//      ofn.nMaxFile     = MAX_PATH;
	//      ofn.lpstrTitle   = "Select a File, yo!";
	//      ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	 //   GetOpenFileNameA( &ofn );

        auto cmdProcess = TinyProcessLib::Process(
            LR"(C:\Windows\System32\cmd.exe)", {},  // Launch cmd.exe
        [](const char *output, size_t n) { std::cout << std::string(output, n); },  // Capture stdout
        [](const char *error, size_t n) { std::cerr << std::string(error, n); },   // Capture stderr
        true);

		cmdProcess.write( "\"" + std::string(R"(C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat)") + "\"");
		cmdProcess.write("\n");
		Sleep(100);
		cmdProcess.write(std::format("cl.exe /std:c++latest /O2 /LD /I {} \"{}\" /link /DLL", "../", sourceFile ));
		cmdProcess.write("\n");
		Sleep(100);
		cmdProcess.write("exit\n");
        cmdProcess.get_exit_status();
	}
}
