#pragma once
#include <Core/src/spa/Vec3.h>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <vector>
#include <memory>
#include <Core/src/gfx/Mesh/Mesh.h>
#include <Core/src/gfx/Mesh/StaticMeshPool.h>
#include <Core/src/gfx/Gfx.h>

namespace tryn::ent
{
	class IEntity
	{
	public:
		virtual ~IEntity() {}
		void Draw(gfx::IGraphics& gfx);
		void SetConstantBuffer(std::shared_ptr<gfx::IConstantBuffer> pCb)
		{
			pConstantBuffer = pCb;
		}
		auto& GetConstantBuffer()
		{
			return *pConstantBuffer;
		}
		auto& GetVertexBuffer()
		{
			return *pVertexBuffer;
		}
		auto& GetIndexBuffer()
		{
			return *pIndexBuffer;
		}
		void AddBindable(std::shared_ptr<gfx::IBindable> bindable)
		{
			otherBindables.push_back(bindable);
		}

	protected:
		std::shared_ptr<gfx::IConstantBuffer> pConstantBuffer;
		std::shared_ptr<gfx::IPolyVBuffer> pVertexBuffer;
		std::shared_ptr<gfx::IIndexBuffer> pIndexBuffer;
		std::shared_ptr<gfx::Mesh> mesh;
		std::vector<std::shared_ptr<gfx::IBindable>> otherBindables;
	};

	class StaticObject : public IEntity
	{
	public:
		StaticObject(std::shared_ptr<gfx::Mesh> mesh)
		{
			trynass_msg(mesh->IsStatic(), L"Cant create a static object with a non static mesh!");
			this->mesh = mesh;
			pVertexBuffer = mesh->GetPolyVBufer();
			pIndexBuffer = mesh->GetIndexBuffer();
		}
		StaticObject(std::string path, int size = 1)
		{
			mesh = gfx::StaticMeshPool::Resolve(path);
			pVertexBuffer = mesh->GetPolyVBufer();
			pIndexBuffer = mesh->GetIndexBuffer();
		}
		void ExpandMeshVertexBuffer(gfx::IGraphics& gfx, gfx::BufferArray& buffArray)
		{
			trynass_msg(mesh != nullptr, L"Cant expand a mesh vertex buffer without a mesh!");
			usingMeshVB = false;
			gfx::BufferArray bfarray;
			std::stringstream ss;
			ss << "ExpandedFrom:" << mesh->GetTag() << "#Added:{";

			for (auto& element : buffArray)
			{
				if (std::holds_alternative<std::shared_ptr<gfx::IVertexBuffer>>(element))
				{
					decltype(auto) ptr = std::get<std::shared_ptr<gfx::IVertexBuffer>>(element);
					bfarray.push_back(ptr);
					ss << ptr->GetTag() << ",";
				}
				else if (std::holds_alternative<std::shared_ptr<gfx::IPolyVBuffer>>(element))
				{
					decltype(auto) ptr = std::get<std::shared_ptr<gfx::IPolyVBuffer>>(element);
					bfarray.push_back(ptr);
					ss << ptr->GetTag() << ",";
				}
				else
				{
					auto& [tag, pVB] = std::get<std::pair<std::string, std::shared_ptr<gfx::VertexBuffer>>>(element);
					auto ptr = gfx::IVertexBuffer::Resolve(gfx, pVB, tag);
					bfarray.push_back(ptr);
					ss << tag;
				}
				ss << "}";
				bfarray.push_back(mesh->GetPolyVBufer());
			}

			pVertexBuffer = gfx::IPolyVBuffer::Resolve(gfx, bfarray, ss.str());
		}
	private:
		bool usingMeshVB = true;
	};
}