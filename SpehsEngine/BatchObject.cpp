
#include "BatchObject.h"
#include "ShaderManager.h"
#include "Console.h"


namespace SpehsEngine
{
	BatchObject::BatchObject()
	{
		Vertex* worldVertexArray = nullptr;
		GLuint numVertices = 0;
		textureDataID = 0;
		readyForDelete = false;
		renderState = true;
		useColor = false;
		shaderIndex = DefaultPolygon;
		scaledMatrix = glm::mat4(1.0f);
		scaledRotatedMatrix = glm::mat4(1.0f);
		vertexArray = nullptr;
		batch = nullptr;
	}
	BatchObject::~BatchObject()
	{
		if (worldVertexArray != nullptr)
			delete [] worldVertexArray;
		if (vertexArray != nullptr)
			delete [] vertexArray;
	}

	void BatchObject::destroy()
	{
		//If this has been already called before or if there is no batch that is going to handle the removal:
		if (readyForDelete == true || batch == nullptr)
		{
			delete this;
			return;
		}
		else
		{
			readyForDelete = true;
		}
	}

	void BatchObject::setRenderState(const bool _newState)
	{
		renderState = _newState;
	}

	void BatchObject::setShader(const int &_newShaderIndex)
	{
		if (shaderManager->getShader(_newShaderIndex) == nullptr)
		{
			console->error("Trying to set a non-existing shader to BatchObject!");
			return;
		}
		shaderIndex = _newShaderIndex;
	}
}
