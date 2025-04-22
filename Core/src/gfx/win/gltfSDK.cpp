#include "TrynPCH.h"
#include "gltfSDK.h"

#include <GLTFSDK/Deserialize.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <filesystem>

using namespace Microsoft::glTF;

namespace tryn::gfx
{
	class GLBStreamReader : public IStreamReader
{
	public:
		GLBStreamReader(std::shared_ptr<std::istream> wrapped) : m_stream(wrapped)
		{
		}

		std::shared_ptr<std::istream> GetInputStream(const std::string&) const override
		{
			// Here we need to translate the string passed into a stream and that is where 
			// we can't progress as we would need arbitrary access to the file system...

			return m_stream;
		}

	private:
		std::shared_ptr<std::istream> m_stream;
	};

	WinGLTFLoaderContext::~WinGLTFLoaderContext() = default;

	WinGLTFLoaderContext WinGLTFLoader::Load(const std::filesystem::path& path)
	{
		std::shared_ptr<IStreamReader> streamReader;
		std::ifstream file(path.string());
		std::shared_ptr<std::istream> pFile = std::make_shared<std::istream>(file.rdbuf());
		std::string jsonStr;
		
		std::unique_ptr<GLTFResourceReader> pReader;
		try
		{
			streamReader = std::make_unique<GLBStreamReader>(pFile);
			auto reader = std::make_unique<GLBResourceReader>(streamReader, pFile);
			jsonStr = reader->GetJson();
			pReader.reset(reader.release());
		}
		catch (GLTFException ex)
		{
			pFile->seekg(0);
			auto it = std::istreambuf_iterator<char>(*(pFile.get()));
			jsonStr = std::string(it, {});
			auto reader = std::make_unique<GLTFResourceReader>(streamReader);
			pReader.reset(reader.release());
		}

		return WinGLTFLoaderContext{.pDocument = std::make_unique<Document>(Deserialize(jsonStr)), .pReader = std::move(pReader)};
	}
}
