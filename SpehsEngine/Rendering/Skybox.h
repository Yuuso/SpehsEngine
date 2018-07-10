
#pragma once

#include <string>


namespace spehs
{
	class Mesh;
	class BatchManager3D;

	class SkyBox
	{
	public:
		SkyBox(BatchManager3D& _batchManager);
		SkyBox(BatchManager3D& _batchManager,
				const std::string& _negx, const std::string& _posx,
				const std::string& _negy, const std::string& _posy,
				const std::string& _negz, const std::string& _posz);
		SkyBox(BatchManager3D& _batchManager, const std::string& _filepath, const std::string& _fileEnding);
		SkyBox(BatchManager3D& _batchManager, const size_t& _hash);
		~SkyBox();

		void setCubeMap(const std::string& _negx, const std::string& _posx,
						const std::string& _negy, const std::string& _posy,
						const std::string& _negz, const std::string& _posz);
		void setCubeMap(const std::string& _filepath, const std::string& _fileEnding); // This automatically finds the 6 files if they are correctly named (e.g. _filepath + "negx" + _fileEnding)
		void setCubeMap(const size_t& _hash); // Uses texture managers hashes to find cube map

		void setShader(const unsigned int _index);

		size_t getCubeMapHash(){ return cubeMapHash; }

	private:
		Mesh* mesh;
		size_t cubeMapHash;
	};

}