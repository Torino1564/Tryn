#include <Core/src/gfx/Bindables/ConstantBufferResource.h>

namespace tryn::gfx
{
	std::string IConstantBufferResource::GenerateID(const IGraphics& gfx, const ConstantBufferLayout& cbl, Type type, int slot, const std::string& tag)
	{
		if (tag == "?") return tag;
		decltype(auto) typeStr = gfx.GetAPIString();
		std::stringstream ss;
		ss << typeStr << "#";
		switch (type)
		{
		case Type::Pixel:
			ss << "PxConstantBuffer#";
			break;
		case Type::Vertex:
			ss << "VtxConstantBuffer#";
			break;
		default:
			trynass_fail(L"Invalid constant buffer type");
		}
		ss << slot << '#' << tag;
		return ss.str();
	}

	std::shared_ptr<IConstantBufferResource> IConstantBufferResource::Resolve(const IGraphics& gfx, const ConstantBufferLayout& cbl, Type type, int slot, const std::string& tag)
	{
		return BindablePool::Resolve<IConstantBufferResource>(gfx, cbl, type, slot, tag);
	}

	void IConstantBufferResource::Accept_(TechniqueProbe& probe) const
	{
		if (probe.VisitBuffer(GetConstantBuffer()))
		{
			pCPUBuffer->SetDirty();
		}
	}

	ConstantBuffer& IConstantBufferResource::GetConstantBuffer()
	{
		pCPUBuffer->SetDirty();
		return static_cast<ConstantBuffer&>(*pCPUBuffer);
	}

	const ConstantBuffer& IConstantBufferResource::GetConstantBuffer() const
	{
		return static_cast<const ConstantBuffer&>(*pCPUBuffer);
	}

	std::string_view IConstantBufferResource::GetPath() const
	{
		return path;
	}

	std::string_view IConstantBufferResource::GetTag() const
	{
		return tag;
	}

	ElementView IConstantBufferResource::operator[](const std::string& id) const
	{
		return GetConstantBuffer()[id];
	}
}