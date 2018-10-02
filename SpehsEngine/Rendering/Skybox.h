
#pragma once

#include <string>


namespace se
{
	namespace rendering
	{
		class Model;
		class BatchManager3D;
		class ModelManager;
		class TextureManager;

		class Skybox
		{
		public:
			Skybox(BatchManager3D& _batchManager, ModelManager& _modelManager, const std::string& _model);
			Skybox(BatchManager3D& _batchManager, ModelManager& _modelManager, TextureManager& _textureManager, const std::string& _model,
				const std::string& _negx, const std::string& _posx,
				const std::string& _negy, const std::string& _posy,
				const std::string& _negz, const std::string& _posz);
			Skybox(BatchManager3D& _batchManager, ModelManager& _modelManager, TextureManager& _textureManager,
				const std::string& _model, const std::string& _filepath, const std::string& _fileEnding);
			Skybox(BatchManager3D& _batchManager, ModelManager& _modelManager, TextureManager& _textureManager,
				const std::string& _model, const size_t& _hash);
			~Skybox();

			void setCubeMap(TextureManager& _textureManager, const std::string& _negx, const std::string& _posx,
				const std::string& _negy, const std::string& _posy,
				const std::string& _negz, const std::string& _posz);
			void setCubeMap(TextureManager& _textureManager, const std::string& _filepath, const std::string& _fileEnding); // This automatically finds the 6 files if they are correctly named (e.g. _filepath + "negx" + _fileEnding)
			void setCubeMap(TextureManager& _textureManager, const size_t& _hash); // Uses texture managers hashes to find cube map

			void setShader(const unsigned int _index);

		private:
			Skybox(const Skybox& _other) = delete;
			Skybox(const Skybox&& _other) = delete;
			Skybox& operator=(const Skybox& _other) = delete;
			Skybox& operator=(const Skybox&& _other) = delete;

			Model* model;
		};
	}
}