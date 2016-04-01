#include "StringOperations.h"
#include "ModelManager.h"
#include "Console.h"
#include <fstream>
#include <glm/vec2.hpp>

spehs::ModelManager* modelManager;
namespace spehs
{
	ModelManager::ModelManager()
	{

	}
	ModelManager::~ModelManager()
	{

	}
	void ModelManager::loadOBJ(const std::string &_filepath, Vertex* vertexArray, std::vector<GLushort> &_elementArray, std::vector<glm::vec3> _normalArray)
	{
		/* .obj files can contain, with the given prefix
		# comment
		v vertex data x,y,z,(w = 1) (+ color value r,g,b)
		vt texture coordinates u,v,(w = 1)
		vn vertex normals x,y,z
		pm parameter space vertices u,(v),(w)
		f polygonal face elements
		*/

		
		std::ifstream stream(_filepath);
		if (!stream.is_open())
		{
			spehs::console::warning("Failed to open OBJ file: " + _filepath);
			return;
		}

		std::string line;
		std::vector<spehs::Vertex> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<GLushort> normalIndices;
		std::vector<GLushort> vertexIndices;
		std::vector<GLushort> textureCoordinateIndices;

		while (true)
		{
			if (stream.eof())
				break;

			//Read the next line
			std::getline(stream, line);

			if (line[0] != '#' && line.size() >= 3/*3+ characters must be on the line in order for it to contain any useful data*/)
			{
				//Read data type from the beginning of the line
				if (line[0] == 'v')
				{
					if (line[1] == ' ' || line[1] == '\t')
					{//Read vertex data "v "...
						vertices.push_back(spehs::Vertex());
						std::string strValue;//Currently reading value as string, to be converted into float using getStringAsFloat()
						int valueIndex = 0;//Index of modified position value x=0, y=1, z=2
						for (unsigned i = 2/*begins with "v "*/; i < line.size(); i++)
						{
							switch (line[i])
							{
							default:
								strValue += line[i];//Record character into value string
								break;
							case ' ':
							case '\t':
								switch (valueIndex)
								{//Assign value to position
								case 0: vertices.back().position.x = getStringAsFloat(strValue); break;
								case 1: vertices.back().position.y = getStringAsFloat(strValue); break;
								}
								strValue.clear();
								++valueIndex;
							}
						}
						vertices.back().position.z = getStringAsFloat(strValue);
					}
					else if (line[1] == 't' && line.size() >= 3 && (line[2] == ' ' || line[1] == '\t'))
					{//Read texture coordinate data "vt "...

					}
					else if (line[1] == 'n' && line.size() >= 3 && (line[2] == ' ' || line[1] == '\t'))
					{//Read vertex normal data "vn "...

					}
				}
				else if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t'))
				{//Read face data "f "...

				}
			}
			//Proceed to read the next line
		}

		stream.close();
	}

}