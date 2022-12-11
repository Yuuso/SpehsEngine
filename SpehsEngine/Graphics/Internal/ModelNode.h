#pragma once


namespace se
{
	namespace graphics
	{
		class Mesh;
		class Model;
		class Primitive;

		class ModelNode
		{
		public:
			ModelNode() = delete;
			ModelNode(const Model& _model, const ModelNode* _parent);

			glm::mat4 getTransform() const;
			void foreachMesh(std::function<void(Primitive&)> _fn);
			void foreachMesh(std::function<void(Primitive&, std::string_view)> _fn);
			const Mesh* getMesh(const std::string_view _name) const;
			const Mesh* getMesh(size_t& _counter) const;
			Mesh* getMesh(const std::string_view _name);
			Mesh* getMesh(size_t& _counter);
			void reset();

			std::string name;
			glm::mat4 transform;
			std::vector<std::unique_ptr<Mesh>> meshes;
			std::vector<std::unique_ptr<ModelNode>> children;

		private:

			glm::mat4 getLocalTransform() const;

			const Model& model;
			const ModelNode* parent;
		};
	}
}
