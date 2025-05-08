#pragma once
#include <d3d11.h>
#include <Core/src/utl/Exception.h>
#include "Serializer.h"
#include <span>
#include <Core/src/ser/ExtraDataPack.h>

class std::ostringstream;
class std::istringstream;

namespace tryn::ser
{
	ZT_EX_DEF(StreamIOException);

	class StreamIO
	{
	public:
		enum Type
		{
			Reader,
			Writer
		};

		StreamIO(const Type type) : type{type} {}

		template <typename T>
		void Field(T* pData, bool binary, const std::string& name);
		virtual void GetAndFill(const std::string&, void*& pToFill) {}
		template <Type T, typename Func>
		void Exclusive(const Func& func)
		{
			if (type == T)
			{
				func();
			}
		}
	private:
		Type type;
	};


	class StreamWriter : public StreamIO
	{
	public:
		explicit StreamWriter(std::shared_ptr<std::ostringstream> oss)
			: StreamIO(Writer), oss(oss) {}

		explicit StreamWriter(StreamWriter&& rhs) noexcept : StreamIO(Writer), oss(std::move(rhs.oss)) {}
		StreamWriter& operator=(StreamWriter&& rhs) noexcept
		{
			oss = std::move(rhs.oss);
			return *this;
		}

		template <typename T>
		requires Serializable<T>
		void Write(T* data, const bool binary = true, const std::string& name = "")
		{
			if constexpr (HasFunctionSerializer<T>)
			{
				return Serialize(*this, data, binary, name);
			}
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				if (binary)
				{
					const auto pData = (char*)(&data);
					WriteBinary(pData, sizeof(T));
				}
			}
		}

		auto& GetStringStream() const
		{
			return *oss;
		}
	private:
		std::shared_ptr<std::ostringstream> oss;
		void WriteBinary(const char* pData, unsigned size) const;
	};

	class StreamReader : public StreamIO
	{
	public:
		explicit StreamReader(std::shared_ptr<std::istringstream>& iss) : StreamIO(Reader), iss(iss){}
		explicit StreamReader(StreamReader&&) noexcept : StreamIO(Reader), iss(iss) {}
		StreamReader& operator=(StreamReader&& rhs) noexcept
		{
			iss = std::move(rhs.iss);
			return *this;
		}

		template <typename T>
		requires Serializable<T>
		void Read(T* pData, const bool binary = true, const std::string& name = "")
		{
			if constexpr (HasFunctionSerializer<T>)
			{
				return Serialize(*this, pData, binary, name);
			}
			else
			{
				if (binary)
				{
					iss->read((char*)pData, sizeof(T));
				}
				else
				{
					return;
				}
			}
		}

		void ExtractExpression(const std::string& expression, std::span<char> pExtraChars = {}) const;
		std::istringstream& GetStringStream() const;
		void GetAndFill(const std::string&, void*& pToFill) override;
		void AddExtraElement(const ElementDataView& element);

	private:
		void ReadBinary(char* pData, unsigned size) const;
		ExtraDataPack extraDataPack;
		std::shared_ptr<std::istringstream> iss;
	};

	template <typename T>
	void StreamIO::Field(T* pData, bool binary = true, const std::string& name = "")
	{
		if (type == Reader)
		{
			auto pReader = static_cast<StreamReader*>(this);
			pReader->Read<T>(pData, binary, name);
		}
		else
		{
			auto pWriter = static_cast<StreamWriter*>(this);
			pWriter->Write<T>(pData, binary, name);
		}
	}
}

#include "DefaultTypeSerializers.h"
