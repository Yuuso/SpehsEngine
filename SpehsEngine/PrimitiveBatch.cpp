#include "PrimitiveBatch.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "ApplicationData.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

//Camera matrix
extern glm::mat4* projectionMatrix;
extern glm::mat4 staticMatrix;


namespace SpehsEngine
{
	PrimitiveBatch::PrimitiveBatch()
	{
	}
	PrimitiveBatch::~PrimitiveBatch()
	{
	}
	void PrimitiveBatch::setRotation(float _newRotAngle)
	{
		rotation = _newRotAngle;
	}


	void PrimitiveBatch::setColor(int _r, int _g, int _b, int _a)
	{
		color = glm::vec4((float) _r / 255.0f, (float) _g / 255.0f, (float) _b / 255.0f, (float) _a / 255.0f);
	}


	void PrimitiveBatch::setShader(ShaderName _shaderIndex)
	{
		shaderIndex = _shaderIndex;
		if (textureDataID)
			shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;
	}


	ShaderName PrimitiveBatch::getShaderIndex()
	{
		return shaderIndex;
	}
	

	void PrimitiveBatch::setCameraMatrixState(bool _newState)
	{
		cameraMatrixState = _newState;
	}


	//Protected:
	void PrimitiveBatch::uniforms()
	{
		if (projectionMatrix != nullptr && cameraMatrixState)
			shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = *projectionMatrix;
		else
			shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = staticMatrix;
		shaderManager->getShader(shaderIndex)->uniforms->color = color;
		shaderManager->getShader(shaderIndex)->uniforms->transformMatrix = transformMatrix;

		shaderManager->setUniforms(shaderIndex);
	}


	void PrimitiveBatch::applyTranformations()
	{
		translationScalingMatrix = glm::scale(translationMatrix, glm::vec3(scaleX / ((float) applicationData->getWindowWidth() / (float) applicationData->getWindowHeight()), scaleY, 0.0f));
		transformMatrix = glm::rotate(translationScalingMatrix, rotation, rotationVector);
	}
}