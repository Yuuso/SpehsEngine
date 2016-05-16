
#pragma once

#include <string>


namespace spehs
{
	class Mesh;

	class SkyBox
	{
	public:
		SkyBox();
		SkyBox(const std::string& _negx, const std::string& _posx,
				const std::string& _negy, const std::string& _posy,
				const std::string& _negz, const std::string& _posz);
		SkyBox(const std::string& _filepath, const std::string& _fileEnding);
		SkyBox(const size_t& _hash);
		~SkyBox();

		void setCubeMap(const std::string& _negx, const std::string& _posx,
						const std::string& _negy, const std::string& _posy,
						const std::string& _negz, const std::string& _posz);
		void setCubeMap(const std::string& _filepath, const std::string& _fileEnding); //This automatically finds the 6 files if they are correctly named (e.g. _filepath + "negx" + _fileEnding)
		void setCubeMap(const size_t& _hash); //Uses texture managers hashes to find cube map

		void setShader(const int _index);

		size_t getCubeMapHash(){ return cubeMapHash; }

	private:
		Mesh* mesh;
		size_t cubeMapHash;
	};

}
