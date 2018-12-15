#include "stdafx.h"

#include "SpehsEngine/Rendering/Skybox.h"
#include "SpehsEngine/Rendering/Model.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/ShaderManager.h"


namespace se
{
	namespace rendering
	{
		Skybox::Skybox(BatchManager3D& _batchManager, ModelManager& _modelManager, const std::string& _model)
		{
			model = new Model();
			model->loadModel(_modelManager, _model);
			model->setStaticDraw(true);
			model->setShaderIndex((unsigned int)ShaderName::DefaultSkybox);
			model->setBlending(true);
			model->setDepthTest(false);
			model->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			model->setScale(glm::vec3(1.2f, 1.2f, 1.2f)); // random magic scale that seems to sort of (not really) work
			_batchManager.add(*model);
		}
		Skybox::Skybox(BatchManager3D& _batchManager, ModelManager& _modelManager, TextureManager& _textureManager, const std::string& _model,
			const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz)
			: Skybox(_batchManager, _modelManager, _model)
		{
			setCubeMap(_textureManager, _negx, _posx, _negy, _posy, _negz, _posz);
		}
		Skybox::Skybox(BatchManager3D& _batchManager, ModelManager& _modelManager, TextureManager& _textureManager, const std::string& _model, const std::string& _filepath, const std::string& _fileEnding)
			: Skybox(_batchManager, _modelManager, _model)
		{
			setCubeMap(_textureManager, _filepath, _fileEnding);
		}
		Skybox::Skybox(BatchManager3D& _batchManager, ModelManager& _modelManager, TextureManager& _textureManager, const std::string& _model, const size_t& _hash)
			: Skybox(_batchManager, _modelManager, _model)
		{
			setCubeMap(_textureManager, _hash);
		}
		Skybox::~Skybox()
		{
			delete model;
		}


		void Skybox::setCubeMap(TextureManager& _textureManager, const std::string& _negx, const std::string& _posx, const std::string& _negy,
								const std::string& _posy, const std::string& _negz, const std::string& _posz)
		{
			se::rendering::TextureData* data = _textureManager.getCubeMapData(_negx, _posx, _negy, _posy, _negz, _posz);
			model->setTexture(data);
		}
		void Skybox::setCubeMap(TextureManager& _textureManager, const std::string& _filepath, const std::string& _fileEnding)
		{
			se::rendering::TextureData* data = _textureManager.getCubeMapData(_filepath, _fileEnding);
			model->setTexture(data);
		}
		void Skybox::setCubeMap(TextureManager& _textureManager, const size_t& _hash)
		{
			model->setTexture(_textureManager, _hash);
		}

		void Skybox::setShader(const unsigned int _index)
		{
			model->setShaderIndex(_index);
		}
	}
}