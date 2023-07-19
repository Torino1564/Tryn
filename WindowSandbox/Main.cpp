#include <Core/src/win/TrynWin.h>
#include <Core/src/log/Log.h>
#include <Core/src/ioc/Container.h>
#include <Core/src/log/SeverityLevelPolicy.h>
#include <Core/src/win/Boot.h>
#include <Core/src/win/IWindow.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/Gfx.h>
#include <Core/src/utl/Exception.h>
#include <Core/src/utl/Timer.h>
#include "TestApp.h"
#include <iostream>
#include <array>
#include <memory>
#include <format>
#include <ranges>
#include <vector>
#include <Core/src/gfx/Assimp.h>
#include <fstream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtc/matrix_transform.hpp>


using namespace tryn;
using namespace std::chrono_literals;
namespace rn = std::ranges;
namespace vi = rn::views;

void Boot()
{
	log::Boot();

	ioc::Get().Register<log::ISeverityLevelPolicy>([] {
		return std::make_shared<log::SeverityLevelPolicy>(log::Level::Verbose);
		});

	win::Boot();
	gfx::Boot();
}

int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	int nCmdShow)
{
	Boot();

	auto window = ioc::Get().Resolve<win::IWindow>(win::IWindow::IocParams{.size = spa::DimensionsI{ .width = (1280), .height = (720) } });
	window->SetTitle(L"Test WindowApp");

	auto gfx = ioc::Get().Resolve<gfx::IGraphics>(gfx::IGraphics::IocParams{window->GetClientDimensions().width, window->GetClientDimensions().height, window->GetHandle()});

	TestApp app(window, gfx);

	Assimp::Importer importer;
	unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;
	const aiScene* scene = importer.ReadFile("resources\\models\\box.obj", flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Error while loading model: " << importer.GetErrorString() << std::endl;
		return 1;
	}

	aiMesh* mesh = scene->mMeshes[0];
	aiVector3D* vertices = mesh->mVertices;
	aiFace* faces = mesh->mFaces;

	std::ofstream file("outputTest.txt");

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		aiVector3D vertex = vertices[i];
		file << "Vertex " << i + 1 << ": (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace& face = faces[i];
		std::cout << "Face " << i + 1 << ": ";
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			file << face.mIndices[j] << " ";
		}
		file << std::endl;
	}

	try
	{
		app.Go();
	}
	catch (utl::BufferedException e)
	{
		MessageBoxA(nullptr, e.what(), 0u, MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "Unknown error", 0u, MB_OK | MB_ICONEXCLAMATION);
	}

	return 0;
}