#pragma once

#include "SpehsEngine/Rendering/BatchRenderResults.h"
#include "SpehsEngine/Rendering/Primitive.h"
#include "SpehsEngine/Rendering/Text.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Rendering/Depth.h"


#include <vector>
#include <stdint.h>

#define DEFAULT_MAX_BATCH_SIZE 4096

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned short GLushort;



namespace spehs
{
	class Camera2D;
	class BatchManager;

	int getIndexMultiplier(const GLenum &_drawMode, const unsigned int& _batchSize = DEFAULT_MAX_BATCH_SIZE); //Calculate max number of indices


	class Batch
	{
	public:
		Batch(BatchManager& batchManager, const PlaneDepth &_priority, const int &_shaderIndex);
		virtual ~Batch();

		virtual bool check(const Primitive &_primitive){ return false; }
		virtual bool check(const Text &_text){ return false; }

		virtual bool render(BatchRenderResults* results = nullptr) = 0;//NOTE: results are incremented rather than set.
		virtual void push(Primitive* _primitive){}
		virtual void push(Text* _text){}

		PlaneDepth getPriority() const{ return priority; }

		BatchManager& batchManager;

	protected:
		PlaneDepth priority;
		int shaderIndex;

		std::vector<spehs::Vertex> vertices;
		std::vector<GLushort> indices;

		GLuint vertexArrayObjectID;
		GLuint vertexBufferID;
		GLuint indexBufferID;

		bool cameraMatrixState;
	};


	class PrimitiveBatch : public Batch
	{
	public:
		PrimitiveBatch(BatchManager& batchManager, const bool _cameraMatrixState, const PlaneDepth &_priority, const bool _blending,
						const int &_shaderIndex, const GLuint &_textureDataID, const GLenum &_drawMode, float _lineWidth);
		~PrimitiveBatch();

		bool check(const Primitive &_primitive);

		bool render(BatchRenderResults* results = nullptr) override;
		void push(Primitive* _primitive);

	protected:
		bool isEnoughRoom(const unsigned int &_numVertices);
		void initBuffers();
		void updateBuffers();
		void setIndices(const unsigned int &_numVertices);

	private:
		GLuint textureDataID;
		float lineWidth;
		GLenum drawMode;
		bool blending;
	};


	class TextBatch : public Batch
	{
	public:
		TextBatch(BatchManager& batchManager, const bool _cameraMatrixState, const PlaneDepth &_priority, const int &_shaderIndex);
		~TextBatch();

		bool check(const Text &_text);

		bool render(BatchRenderResults* results = nullptr) override;
		void push(Text* _text);

	protected:
		bool isEnoughRoom(const unsigned int &_numVertices);
		void initBuffers();
		void updateBuffers();
		void setIndices();

	private:
		std::vector<GLuint> textureIDs;
	};
}
