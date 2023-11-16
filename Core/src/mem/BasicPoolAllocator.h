#pragma once
#include <vector>
#include <bitset>
#include <Core/third/dynamic_bitset.hpp>
#include <Core/src/utl/Assert.h>
#include <memory>

namespace tryn::mem
{
	template <typename Bucket = std::size_t, typename BitSetBucket = std::size_t>
	class BasicPoolAllocator
	{
	public:
		BasicPoolAllocator(std::size_t numBuckets)
			:
			numBuckets(numBuckets)
		{
			buffer.resize(static_cast<std::size_t>(numBuckets * bytesPerBucket), (std::byte)(0));
			booking.resize(numBuckets, true);
		}
		std::byte* Request(std::size_t numBytes)
		{
			bool found = false;
			std::size_t index = 0;
			std::size_t byteIndex = 0;
			std::size_t inByteOffset = 0;
			auto numBuckets;
			const auto divResult = std::div(numBytes, static_cast<int>(bytesPerBucket));
			if (divResult.rem != 0)
			{
				numBuckets = divResult.quot + 1;
			}
			else
			{
				numBuckets = divResult.quot;
			}

			while (!found)
			{
				index = booking.find_next(index);
				const auto divResult = std::div(index, static_cast<int>(bytesPerBitsetBucket * 8));
				byteIndex = divResult.quot;
				inByteOffset = divResult.rem;

				auto bytes = booking.data();
				auto& byteProxy = bytes[byteIndex];

				byteProxy << inByteOffset;

				trynass(byteProxy & GetMask());

				for (int i = 0; i < numBuckets - 1; i++)
				{
					if ((inByteOffset + i) > bytesPerBitsetBucket * 8)
					{
						byteProxy = bytes[++byteIndex];
						inByteOffset = 0;
					}
					byteProxy << 1;

					if (!byteProxy & GetMask())
					{
						break;
					}
				}
				found = true;
			}
			// If found, mark those bytes as taken;
			for(int i = 0 ; i < numBuckets ; i++)
			{
				booking[index + i] = false;
			}

			return &buffer[index * bytesPerBucket];
		}
		void Return(std::size_t index, std::size_t byteNumber)
		{
			trynass(index + byteNumber < numBuckets);
			for (int i = 0 ; i < byteNumber ; )
			{
				booking[index++] = true;
			}
		}
		static BitSetBucket& GetMask()
		{
			static BitSetBucket mask = ~static_cast<BitSetBucket>(1 << (sizeof(BitSetBucket) * 8 - 1));
			return mask;
		}
	private:
		//Booking
		sul::dynamic_bitset<BitSetBucket> booking;
		//Data
		std::size_t numBuckets;
		static constexpr std::size_t bytesPerBitsetBucket = sizeof(BitSetBucket);
		static constexpr std::size_t bytesPerBucket = sizeof(Bucket);
		std::vector<std::byte> buffer;
	};
}