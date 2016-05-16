
#include "SkyBox.h"
#include "BatchManager.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "ShaderManager.h"	


namespace spehs
{
	SkyBox::SkyBox()
	{
		mesh = getActiveBatchManager()->createMesh();
		mesh->setShader(DefaultSkyBox);
		mesh->setMesh("InvertedSphere");
		mesh->setBlending(true);
		mesh->setPosition(0.0f, 0.0f, 0.0f);
	}
	SkyBox::SkyBox(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy, const std::string& _negz, const std::string& _posz) : SkyBox()
	{
		setCubeMap(_negx, _posx, _negy, _posy, _negz, _posz);
	}
	SkyBox::SkyBox(const std::string& _filepath, const std::string& _fileEnding) : SkyBox()
	{
		setCubeMap(_filepath, _fileEnding);
	}
	SkyBox::SkyBox(const size_t& _hash) : SkyBox()
	{
		setCubeMap(_hash);
	}
	SkyBox::~SkyBox()
	{
		mesh->destroy();
	}


	void SkyBox::setCubeMap(const std::string& _negx, const std::string& _posx, const std::string& _negy, const std::string& _posy,	const std::string& _negz, const std::string& _posz)
	{
		size_t hash = textureManager->preloadCubeMap(_negx, _posx, _negy, _posy, _negz, _posz);
		setCubeMap(hash);
	}
	void SkyBox::setCubeMap(const std::string& _filepath, const std::string& _fileEnding)
	{
		size_t hash = textureManager->preloadCubeMap(_filepath + "negx" + _fileEnding, _filepath + "posx" + _fileEnding,
														_filepath + "negy" + _fileEnding, _filepath + "posy" + _fileEnding,
														_filepath + "negz" + _fileEnding, _filepath + "posz" + _fileEnding);
		setCubeMap(hash);
	}
	void SkyBox::setCubeMap(const size_t& _hash)
	{
		cubeMapHash = _hash;
		mesh->setTexture(_hash);
	}

	void SkyBox::setShader(const int _index)
	{
		mesh->setShader(_index);
	}
}