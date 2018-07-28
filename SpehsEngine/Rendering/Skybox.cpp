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
		Skybox::Skybox(BatchManager3D& _batchManager, const std::string& _model)
			: batchManager(&_batchManager)
		{
			model = new Model();
			batchManager->add(*model);
			model->loadModel(_model);
			model->setShaderIndex((unsigned int)ShaderName::DefaultSkybox);
			model->setBlending(true);
			model->setDepthTest(false);
			model->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			model->setScale(glm::vec3(1.2f, 1.2f, 1.2f)); // random magic scale that seems to sort of (not really) work
		}
		Skybox::Skybox(BatchManager3D& _batchManager, const std::string& _model,
			const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz)
			: Skybox(_batchManager, _model)
		{
			setCubeMap(_negx, _posx, _negy, _posy, _negz, _posz);
		}
		Skybox::Skybox(BatchManager3D& _batchManager, const std::string& _model, const std::string& _filepath, const std::string& _fileEnding)
			: Skybox(_batchManager, _model)
		{
			setCubeMap(_filepath, _fileEnding);
		}
		Skybox::Skybox(BatchManager3D& _batchManager, const std::string& _model, const size_t& _hash)
			: Skybox(_batchManager, _model)
		{
			setCubeMap(_hash);
		}
		Skybox::~Skybox()
		{
			delete model;
		}


		void Skybox::setCubeMap(const std::string& _negx, const std::string& _posx, const std::string& _negy,
								const std::string& _posy, const std::string& _negz, const std::string& _posz)
		{
			size_t hash = batchManager->textureManager.preloadCubeMap(_negx, _posx, _negy, _posy, _negz, _posz);
			setCubeMap(hash);
		}
		void Skybox::setCubeMap(const std::string& _filepath, const std::string& _fileEnding)
		{
			// WHY CAN'T I USE MODELS BATCHMANAGER!?
			size_t hash = batchManager->textureManager.preloadCubeMap(_filepath + "negx" + _fileEnding, _filepath + "posx" + _fileEnding,
				_filepath + "negy" + _fileEnding, _filepath + "posy" + _fileEnding,
				_filepath + "negz" + _fileEnding, _filepath + "posz" + _fileEnding);
			setCubeMap(hash);
		}
		void Skybox::setCubeMap(const size_t& _hash)
		{
			model->setTexture(_hash);
		}

		void Skybox::setShader(const unsigned int _index)
		{
			model->setShaderIndex(_index);
		}
	}
}