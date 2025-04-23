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

	class StreamReader : public IStreamReader
	{
	public:
		StreamReader(std::filesystem::path pathBase) : m_pathBase(std::move(pathBase))
		{

		}

		// Resolves the relative URIs of any external resources declared in the glTF manifest
		std::shared_ptr<std::istream> GetInputStream(const std::string& filename) const override
		{
			// In order to construct a valid stream:
			// 1. The filename argument will be encoded as UTF-8 so use filesystem::u8path to
			//    correctly construct a path instance.
			// 2. Generate an absolute path by concatenating m_pathBase with the specified filename
			//    path. The filesystem::operator/ uses the platform's preferred directory separator
			//    if appropriate.
			// 3. Always open the file stream in binary mode. The glTF SDK will handle any text
			//    encoding issues for us.
			auto streamPath = m_pathBase / std::filesystem::path(filename);
			auto stream = std::make_shared<std::ifstream>(streamPath, std::ios_base::binary);

			// Check if the stream has no errors and is ready for I/O operations
			if (!stream || !(*stream))
			{
				throw std::runtime_error("Unable to create a valid input stream for uri: " + filename);
			}

			return stream;
		}

	private:
		std::filesystem::path m_pathBase;
	};

	void WinGLTFLoader::Load(const std::filesystem::path& path, const std::function<void(const WinGLTFLoaderContext&)>& process)
	{
		auto streamReader = std::make_unique<StreamReader>(path.parent_path());
		std::string manifest;
		std::unique_ptr<GLTFResourceReader> pReader;
		try
		{
			auto glbStream = streamReader->GetInputStream(path.filename().string());
			auto reader = std::make_unique<GLBResourceReader>(std::move(streamReader), std::move(glbStream));
			manifest = reader->GetJson();
			pReader = std::move(reader);
		}
		catch (GLTFException ex)
		{
			auto gltfStream = streamReader->GetInputStream(path.filename().string());
			auto reader = std::make_unique<GLTFResourceReader>(std::move(streamReader));

			std::stringstream manifestStream;

			manifestStream << gltfStream->rdbuf();
			manifest = manifestStream.str();

			pReader = std::move(reader);
		}

		const WinGLTFLoaderContext context{.pDocument = std::make_unique<Document>(Deserialize(manifest)), .pReader = std::move(pReader)};
		process(context);
	}
}
