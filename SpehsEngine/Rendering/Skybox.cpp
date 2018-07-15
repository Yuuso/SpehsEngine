#include "stdafx.h"

#include "SkyBox.h"
#include "BatchManager3D.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "ShaderManager.h"


namespace se
{
	namespace rendering
	{
		SkyBox::SkyBox(BatchManager3D& _batchManager)
		{
			mesh = new Mesh(_batchManager);
			mesh->setShaderIndex((unsigned int)ShaderName::DefaultSkybox);
			mesh->setMesh("InvertedSphere");
			mesh->setBlending(true);
			mesh->setDepthTest(false);
			mesh->setPosition(0.0f, 0.0f, 0.0f);
			mesh->setScale(1.2f); // random magic scale that seems to work
		}
		SkyBox::SkyBox(BatchManager3D& _batchManager, const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz) : SkyBox(_batchManager)
		{
			setCubeMap(_negx, _posx, _negy, _posy, _negz, _posz);
		}
		SkyBox::SkyBox(BatchManager3D& _batchManager, const std::string& _filepath, const std::string& _fileEnding) : SkyBox(_batchManager)
		{
			setCubeMap(_filepath, _fileEnding);
		}
		SkyBox::SkyBox(BatchManager3D& _batchManager, const size_t& _hash) : SkyBox(_batchManager)
		{
			setCubeMap(_hash);
		}
		SkyBox::~SkyBox()
		{
			delete mesh;
		}


		void SkyBox::setCubeMap(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz)
		{
			size_t hash = mesh->batchManager->textureManager.preloadCubeMap(_negx, _posx, _negy, _posy, _negz, _posz);
			setCubeMap(hash);
		}
		void SkyBox::setCubeMap(const std::string& _filepath, const std::string& _fileEnding)
		{
			size_t hash = mesh->batchManager->textureManager.preloadCubeMap(_filepath + "negx" + _fileEnding, _filepath + "posx" + _fileEnding,
				_filepath + "negy" + _fileEnding, _filepath + "posy" + _fileEnding,
				_filepath + "negz" + _fileEnding, _filepath + "posz" + _fileEnding);
			setCubeMap(hash);
		}
		void SkyBox::setCubeMap(const size_t& _hash)
		{
			cubeMapHash = _hash;
			mesh->setTexture(_hash);
		}

		void SkyBox::setShader(const unsigned int _index)
		{
			mesh->setShaderIndex(_index);
		}
	}
}