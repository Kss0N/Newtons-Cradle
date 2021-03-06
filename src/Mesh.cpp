#include "Mesh.h"
#include "CradleCradle.h"
#include "Camera.h"
#include <istream>
#include <fstream>



void Mesh::tansferBufferToGL(GLuint inVAO) {	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_VERTEX_ARRAY, VBO);
	glBufferData(GL_VERTEX_ARRAY,
		//0th vertex contains nothing
		vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);


	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)0);
	glEnableVertexAttribArray(0);
	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)(sizeof(PxVec3)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

	


/*obj file type*/
Mesh::Mesh(std::string& meshPath) {
	using namespace std;
	Vertex stdVertex = {
		.pos = PxVec3(0,0,0),
		.normal = PxVec3(0,0,0),
	};

	std::vector<Vertex> vertices;
	vertices.push_back(stdVertex); //to make sure things actually work 
	std::vector<PxVec3> normals;
	normals.push_back(physx::PxVec3(0, 0, 0));
	std::vector<PxVec2> textures;
	textures.push_back(physx::PxVec2(0, 0));
	std::vector<unsigned int> indices;

	auto vertIndex = +1;
	auto normIndex = +1;
	auto textIndex = +1;
	auto elemIndex = +1;

	std::string line;
	std::ifstream file(meshPath.c_str());
	if (file.fail()) {
		exit(1);
	}

	bool bFailFlag = false;
	while (!bFailFlag && getline(file, line)) {

		/*Comment, ignore*/
		if (line[0] == '#') continue;

		/*Vertices*/
		else if (line[0] == 'v') {
			switch (line[1])
			{
			case ' ':
			{
				/*Geom Vertex*/
				Vertex vert;
				sscanf_s(line.c_str(), "v %f %f %f",
					&(vert.pos.x),
					&(vert.pos.y),
					&(vert.pos.z));
				vertices.push_back(vert);
			}
			case 'n':
			{
				PxVec3 normal;
				sscanf_s(line.c_str(), "vn %f %f %f",
					&(normal.x),
					&(normal.y),
					&(normal.z));
				normals.push_back(normal);
			}
			case 't':
			{
				PxVec2 uvCoord;
				sscanf_s(line.c_str(), "vt %f %f",
					&(uvCoord.x),
					&(uvCoord.y));
				textures.push_back(uvCoord);
			}
			default:
				//Invalid, return null
				break;
			}
		}

		else if (line[0] == 'f')
		{
			/*
			*A Face is a polygon (contains three or more vertices)
			*OpenGL for all intents and purposes here only accepts triangles
			*Each polygon consists of indices to vertices which compose the polygon
			*
			*/
			unsigned int geomElements[3];
			unsigned int normElements[3];
			unsigned int textElements[3];
			/*geometry/texture/normal*/
			if (sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
				geomElements + 0, textElements + 0, normElements + 0,
				geomElements + 1, textElements + 1, normElements + 1,
				geomElements + 2, textElements + 2, normElements + 2) == 2)
			{

			}
			else if (sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d",
				geomElements + 0, textElements + 0,
				geomElements + 1, textElements + 1,
				geomElements + 2, textElements + 2) == 2)
			{
				memset(normElements, 0, sizeof(normElements));
			}
			else if (sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d",
				geomElements + 0, normElements + 0,
				geomElements + 1, normElements + 1,
				geomElements + 2, normElements + 2) == 2)
			{
				memset(textElements, 0, sizeof(textElements));
			}



			/*Insert indices; DEAL WITH IT*/
			if (normElements[0] != 0) {
				vertices[geomElements[0]].normal = normals[normElements[0]];
				vertices[geomElements[1]].normal = normals[normElements[1]];
				vertices[geomElements[2]].normal = normals[normElements[2]];
			}
			if (textElements[0] != 0) {
				/*
				vertices[geomElements[0]].uvCoord = textures[textElements[0]];
				vertices[geomElements[1]].uvCoord = textures[textElements[1]];
				vertices[geomElements[2]].uvCoord = textures[textElements[2]];
				*/
			}
			indices.push_back(geomElements[0]);
			indices.push_back(geomElements[1]);
			indices.push_back(geomElements[2]);

		}
	}
	this->vertices = vertices;
	this->indices = indices;

	if (bFailFlag) {
		return;
	}

	
}



void Mesh::draw(Shader& shader, PxVec3 worldPos) 
{
	glUseProgram(shader);
	glBindVertexArray(VAO);

	//glUniform3f(glGetUniformLocation(shader, "orginPos"), worldPos.x, worldPos.y, worldPos.z);
	//auto camMatrix = camera.cameraMatrix.front();
	//glUniformMatrix4fv(glGetUniformLocation(shader, "camMatrix"), 1, FALSE, camMatrix);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}






physx::PxTriangleMeshGeometry Mesh::createPxGeometry(double scale) {
	using namespace physx;
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.stride = sizeof(Vertex);
	meshDesc.points.count = vertices.size()-1;
	//I'm guessing the data field is the begin pos of the vertices, because I chose to start
	meshDesc.points.data = ((char*)vertices.data()+1);

	/*
	*	triangles are the same as the EBO, but it's divided into groups of three.
	*/
	meshDesc.triangles.stride = 3 * sizeof(unsigned int);
	meshDesc.triangles.count = indices.size() / 3;
	meshDesc.triangles.data = indices.data();

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool cookingSuccess = P::gpCooking->cookTriangleMesh
	(meshDesc, writeBuffer, &result);

	if (not cookingSuccess) {

	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	auto mesh = P::gpPhysics->createTriangleMesh(readBuffer);
	PxMeshGeometryFlags flags; //No Flags
	PxTriangleMeshGeometry TriGeom(mesh, PxMeshScale(scale), flags);
	return TriGeom;
}