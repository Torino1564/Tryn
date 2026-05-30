
#include "SinkAndSource.h"
#include <Core/src/gfx/Render/RenderPass.h>

namespace tryn::gfx
{
	void Sink::Bind(Source& source, const std::string& exposure, const std::string& dependency,
	                const std::optional<utl::UUID_t> uuid)
	{
		auto exposureIt = std::ranges::find_if(source.data, [&](const Source::Entry& entry)
			{
				return entry.name == exposure;
			});
		trynass(exposureIt != source.data.end()).msg(L"Could not find exposure: [" + utl::ToWide(exposure) + L"] in pass: [" + (source.pPass != nullptr ? utl::ToWide(source.pPass->GetName()) : L"null") + L"]");

		if (uuid.has_value())
			trynass(exposureIt->uuid == uuid.value());

		// TODO: here is where we should check it only has one const bind. We are not doing that now
		trynass(exposureIt->bindings.size() == 0).msg(L"Attempting to bing to a bounded source");

		auto dependencyIt = std::ranges::find_if(data, [&](const Entry& entry)
			{
				return entry.name == dependency;
			});
		trynass(dependencyIt != data.end()).msg(L"Could not find dependency: " + utl::ToWide(dependency));
		trynass(dependencyIt->pSource == nullptr).msg(L"Dependency [" + utl::ToWide(dependency) + L"] already bounded");

		dependencyIt->pSource = &source;
		dependencyIt->exposureIndex = std::distance(std::begin(source.data), exposureIt);

		// add to binding on the source
		exposureIt->bindings.emplace_back(this, Source::Type::NonConst);
	}

	void* Source::Get(const uint16_t index) const
	{
		return data[index].pResource;
	}
}
