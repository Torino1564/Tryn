#pragma once
#include <memory>
#include <Core/src/gfx/IGraphics.h>
#include <string>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <Core/src/gfx/Bindables/PixelShader.h>

namespace tryn::gfx
{
	enum class Techniques
	{
		PhongFlatColor,
		FlatColor,
	};

	class Technique
	{
	private:
		template <Techniques T>
		struct TechniqueFile {};
		template <> struct TechniqueFile<Techniques::PhongFlatColor>
		{
			static constexpr const char* name = "PhongFlatColor";
		};
		template <> struct TechniqueFile<Techniques::FlatColor>
		{
			static constexpr const char* name = "FlatColor";
		};
	public:
		template <Techniques T>
		static std::shared_ptr<Technique> Resolve(IGraphics& gfx)
		{
			char vsPath[50];
			char psPath[50];

			strcpy(vsPath, TechniqueFile<T>::name);
			static auto vsEnd = "_VS.cso";
			strcat(vsPath, vsEnd);

			strcpy(psPath, TechniqueFile<T>::name);
			static auto psEnd = "_PS.cso";
			strcat(psPath, psEnd);

			return std::make_shared<Technique>(gfx, vsPath, psPath);
		}
		IVertexShader& GetVertexShader()
		{
			return *pVertexShader;
		}
		IPixelShader& GetPixelShader()
		{
			return *pPixelShader;
		}
		Technique(IGraphics& gfx, std::string vsPath, std::string  psPath)
		{
			pVertexShader = IVertexShader::Resolve(gfx, vsPath);
			pPixelShader = IPixelShader::Resolve(gfx, psPath);
		}

		std::shared_ptr<IVertexShader> pVertexShader;
		std::shared_ptr<IPixelShader> pPixelShader;
	};
}