
#include "ControlMesh.h"
#include "Mesh.h"
#include "BatchManager.h"


namespace spehs
{
	ControlMesh::ControlMesh()
	{
	}
	ControlMesh::ControlMesh(Mesh* _mesh)
	{
		mesh = _mesh;
	}
	ControlMesh::ControlMesh(const std::string &_filepath)
	{
		mesh = getActiveBatchManager()->createMesh(_filepath);
	}
	ControlMesh::~ControlMesh()
	{
	}
}