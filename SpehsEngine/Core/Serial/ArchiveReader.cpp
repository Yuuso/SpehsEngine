#include "stdafx.h"
#include "SpehsEngine/Core/Serial/ArchiveReader.h"


namespace se
{
	struct ArchiveReader::Impl
	{
		struct ReadResult
		{
			Container rootContainer;
			std::vector<uint8_t> valueData;
			uint8_t version = 0;
		};

		static inline std::optional<ReadResult> readVersion1(BinaryReader& binaryReader)
		{
			uint32_t valueCount = 0;
			if (!binaryReader.serial(valueCount))
			{
				return std::nullopt;
			}
			uint32_t dataSize = 0;
			if (!binaryReader.serial(dataSize))
			{
				return std::nullopt;
			}

			// Value headers
			std::vector<Value> values;
			std::vector<uint32_t> valueHashes;
			values.resize(size_t(valueCount));
			valueHashes.resize(size_t(valueCount));
			uint32_t valueDataOffset = 0;
			for (uint32_t i = 0; i < valueCount; i++)
			{
				Value& value = values[i];
				if (!binaryReader.serial(valueHashes[i]))
				{
					return std::nullopt;
				}
				if (!binaryReader.serial(value.size))
				{
					return std::nullopt;
				}
				value.index = valueDataOffset;
				valueDataOffset += value.size;
			}

			// Value data
			// NOTE: MSVC seems to have a bug here so the following does not work
			// if (dataSize > binaryReader.getBytesRemaining())
			{
				const size_t bytesRemaining = binaryReader.getBytesRemaining();
				if (dataSize > bytesRemaining)
				{
					return std::nullopt;
				}
			}
			std::vector<uint8_t> valueData;
			valueData.resize(dataSize);
			memcpy(valueData.data(), binaryReader.getData() + binaryReader.getOffset(), dataSize);
			binaryReader.translate(int(dataSize));

			// Containers
			constexpr size_t containerHeaderSize = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
			const size_t containerBytes = binaryReader.getBytesRemaining();
			const uint32_t containerCount = uint32_t(containerBytes / containerHeaderSize);
			if (containerCount * containerHeaderSize != containerBytes)
			{
				se_assert(false);
				return std::nullopt;
			}
			struct ContainerHeader
			{
				uint32_t hash = 0;
				uint32_t valuesBegin = 0;
				uint32_t valuesEnd = 0;
				uint32_t containerCount = 0;
			};
			std::vector<ContainerHeader> containerHeaders;
			containerHeaders.resize(containerCount);
			for (ContainerHeader& containerHeader : containerHeaders)
			{
				if (!binaryReader.serial(containerHeader.hash) ||
					!binaryReader.serial(containerHeader.valuesBegin) ||
					!binaryReader.serial(containerHeader.valuesEnd) ||
					!binaryReader.serial(containerHeader.containerCount))
				{
					return std::nullopt;
				}
				if (containerHeader.valuesBegin > containerHeader.valuesEnd)
				{
					return std::nullopt;
				}
				if (containerHeader.valuesEnd > uint32_t(values.size()))
				{
					return std::nullopt;
				}
				if (containerHeader.containerCount > containerCount)
				{
					return std::nullopt;
				}
			}
			if (binaryReader.getBytesRemaining() != 0)
			{
				se_assert(false);
				return std::nullopt;
			}

			struct StackContainer
			{
				Container* container = nullptr;
				ContainerHeader* header = nullptr;
			};
			ContainerHeader rootContainerHeader;
			rootContainerHeader.hash = 0;
			rootContainerHeader.valuesBegin = 0;
			rootContainerHeader.valuesEnd = valueCount;
			rootContainerHeader.containerCount = 0x13371337; // This just needs to be not 0 for the root folder
			Container rootContainer;
			std::stack<StackContainer> stack;
			stack.push(StackContainer());
			stack.top().container = &rootContainer;
			stack.top().header = &rootContainerHeader;

			size_t nextValueIndex = 0;
			auto advanceValues = [&](const size_t end)
			{
				se_assert(!stack.empty());
				while (nextValueIndex < end && nextValueIndex < stack.top().header->valuesEnd)
				{
					se_assert(nextValueIndex >= stack.top().header->valuesBegin);
					const uint32_t hash = valueHashes[nextValueIndex];
					se_assert(!tryFind(stack.top().container->values, hash));
					stack.top().container->values[hash] = values[nextValueIndex];
					nextValueIndex++;
				}
			};
			auto endContainer = [&]()
			{
				advanceValues(stack.top().header->valuesEnd);
				stack.pop();
			};
			for (ContainerHeader& containerHeader : containerHeaders)
			{
				while (stack.top().header->containerCount == 0)
				{
					endContainer();
				}
				advanceValues(containerHeader.valuesBegin);
				stack.top().header->containerCount--;
				Container& parentContainer = *stack.top().container;
				se_assert(!tryFind(parentContainer.containers, containerHeader.hash));
				stack.push(StackContainer());
				stack.top().container = &parentContainer.containers[containerHeader.hash];
				stack.top().header = &containerHeader;
			}
			while (!stack.empty())
			{
				endContainer();
			}

			// Result
			ReadResult result;
			result.version = 1;
			result.valueData.swap(valueData);
			std::swap(result.rootContainer, rootContainer);
			return result;
		}

		static inline std::optional<ReadResult> readImpl(const uint8_t* const _data, const size_t _size)
		{
			uint8_t version = 0;
			BinaryReader binaryReader(_data, _size);
			if (!binaryReader.serial(version))
			{
				return std::nullopt;
			}
			if (version == 0)
			{
				// Invalid version
				return std::nullopt;
			}
			else if (version == 1)
			{
				if (std::optional<ReadResult> result = readVersion1(binaryReader))
				{
					return result;
				}
				else
				{
					return std::nullopt;
				}
			}
			else
			{
				// Cannot read future versions / the version value is corrupt
				log::error("ArchiveReader failed to read, unhandled version: " + std::to_string(version));
				return std::nullopt;
			}
		}
	};

	ArchiveReader::ArchiveReader(const uint8_t* const _data, const size_t _size)
	{
		if (std::optional<Impl::ReadResult> result = Impl::readImpl(_data, _size))
		{
			std::swap(rootContainer, result->rootContainer);
			std::swap(valueData, result->valueData);
			version = result->version;
		}
		else
		{
			version = 0;
		}
	}
}
