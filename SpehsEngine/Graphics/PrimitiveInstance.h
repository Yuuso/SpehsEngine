#pragma once

#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/Graphics/BatchPosition.h"
#include "SpehsEngine/Graphics/Batch.h"


namespace se
{
	namespace graphics
	{
		class PrimitiveInstance
		{
		public:

			PrimitiveInstance(const Primitive& _primitive);
			~PrimitiveInstance();

			PrimitiveInstance(const PrimitiveInstance& _other) = delete;
			PrimitiveInstance& operator=(const PrimitiveInstance& _other) = delete;

			PrimitiveInstance(PrimitiveInstance&& _other) = delete;
			PrimitiveInstance& operator=(PrimitiveInstance&& _other) = delete;

			bool operator==(const Primitive& _other);


			const Primitive& primitive;
			BatchPosition batchPosition;
			Batch* batch = nullptr;
		};
	}
}
