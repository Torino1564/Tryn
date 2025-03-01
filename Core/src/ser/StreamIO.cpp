#include "TrynPCH.h"
#include "StreamIO.h"

namespace tryn::ser
{
	void StreamReader::ExtractExpression(const std::string& expression, std::span<char> pExtraChars) const
	{
		static std::string charBuffer;

		charBuffer.resize(expression.size());

		std::memset(charBuffer.data(), '0', charBuffer.size());

		iss.read(charBuffer.data(), expression.size());

		if (expression != charBuffer)
			throw StreamIOException{"Failed to parse the requested expression from file"};

		if (pExtraChars.data() != nullptr)
		{
			iss.read(pExtraChars.data(), pExtraChars.size());
		}
	}

	std::istringstream& StreamReader::GetStringStream() const
	{
		return iss;
	}

	void StreamReader::ReadBinary(char* pData, const unsigned size) const
	{
		iss.read(pData, size);
	}

	void StreamWriter::WriteBinary(const char* pData, const unsigned size) const
	{
		oss.write(pData, size);
	}
}
