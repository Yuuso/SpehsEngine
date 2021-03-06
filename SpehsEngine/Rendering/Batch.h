#pragma once

#include "SpehsEngine/Rendering/Primitive.h"
#include "SpehsEngine/Rendering/Text.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Rendering/Depth.h"


#include <vector>
#include <stdint.h>

#define DEFAULT_MAX_BATCH_SIZE 4096u

typedef unsigned int GLuint;
typedef unsigned short GLushort;



namespace se
{
	namespace rendering
	{
		class Camera2D;
		class BatchManager;
		struct BatchRenderResults;

		int getIndexMultiplier(const DrawMode _drawMode, const size_t _batchSize = DEFAULT_MAX_BATCH_SIZE); // Calculate max number of indices


		class Batch
		{
		public:
			Batch(BatchManager& _batchManager, const PlaneDepth _priority, const size_t _shaderIndex, const bool _cameraMatrixState);
			virtual ~Batch();

			virtual bool check(const Primitive& _primitive) { return false; }
			virtual bool check(const Text& _text) { return false; }

			virtual bool render(BatchRenderResults* results = nullptr) = 0; // NOTE: results are incremented rather than set.
			virtual void push(const Primitive& _primitive) {}
			virtual void push(const Text& _text) {}

			PlaneDepth getPriority() const { return priority; }

			BatchManager& batchManager;

		protected:
			PlaneDepth priority;
			size_t shaderIndex;
			bool cameraMatrixState;

			std::vector<se::Vertex> vertices;
			std::vector<GLushort> indices;

			GLuint vertexArrayObjectID = 0;
			GLuint vertexBufferID = 0;
			GLuint indexBufferID = 0;
		};


		class PrimitiveBatch : public Batch
		{
		public:
			PrimitiveBatch(BatchManager& _batchManager, const bool _cameraMatrixState, const PlaneDepth _priority, const bool _blending,
				const int _shaderIndex, const GLuint _textureDataID, const DrawMode _drawMode, const float _lineWidth);
			~PrimitiveBatch();

			bool check(const Primitive& _primitive);

			bool render(BatchRenderResults* results = nullptr) override;
			void push(const Primitive& _primitive);

		protected:
			bool isEnoughRoom(const size_t _numVertices);
			void initBuffers();
			void updateBuffers();
			void setIndices(const size_t _numVertices);

		private:
			GLuint textureDataID;
			float lineWidth;
			DrawMode drawMode;
			bool blending;
		};


		class TextBatch : public Batch
		{
		public:
			TextBatch(BatchManager& _batchManager, const bool _cameraMatrixState, const PlaneDepth _priority, const size_t _shaderIndex);
			~TextBatch();

			bool check(const Text& _text);

			bool render(BatchRenderResults* results = nullptr) override;
			void push(const Text& _text);

		protected:
			bool isEnoughRoom(const size_t _numVertices);
			void initBuffers();
			void updateBuffers();
			void setIndices();

		private:
			std::vector<GLuint> textureIDs;
		};
	}
}
