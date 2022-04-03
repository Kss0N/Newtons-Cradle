#include "Mesh.h"
#include "CradleCradle.h"

#include <istream>
#include <fstream>






/*obj file type*/
Mesh::Mesh(std::string& meshPath, GLuint inVAO) {
	using namespace std;

	std::vector<Vertex> vertices;
	std::vector<PxVec3> normals;
	std::vector<PxVec2> textures;
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
				vertices[vertIndex++] = vert;
			}
			case 'n':
			{
				PxVec3 normal;
				sscanf_s(line.c_str(), "vn %f %f %f",
					&(normal.x),
					&(normal.y),
					&(normal.z));
				normals[normIndex++] = normal;
			}
			case 't':
			{
				PxVec2 uvCoord;
				sscanf_s(line.c_str(), "vt %f %f",
					&(uvCoord.x),
					&(uvCoord.y));
				textures[textIndex++] = uvCoord;
			}
			default:
				//Invalid, return null
				bFailFlag = true;
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
			else if (sscanf_s(line.c_str(), "%d/%d %d/%d %d/%d",
				geomElements + 0, textElements + 0,
				geomElements + 1, textElements + 1,
				geomElements + 2, textElements + 2) == 2)
			{
				memset(normElements, 0, sizeof(normElements));
			}
			else if (sscanf_s(line.c_str(), "%d//%d %d//%d %d//%d",
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
			indices[elemIndex++] = geomElements[0];
			indices[elemIndex++] = geomElements[1];
			indices[elemIndex++] = geomElements[2];
		}
	}
	this->vertices = vertices;
	this->indices = indices;

	if (bFailFlag) {
		return;
	}

	/*Finally the OpenGL parts Enjoy hehe*/
	GLuint bufferObjects[2];
	glGenBuffers(2, bufferObjects);
	VBO = bufferObjects[0];
	EBO = bufferObjects[1];

	bool VAOSupplied = false;
	if (!inVAO) {
		glGenVertexArrays(1, &(VAO));
	}
	else { VAO = inVAO; VAOSupplied = true; }
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_VERTEX_ARRAY, VBO);
	glBufferData(GL_VERTEX_ARRAY,
		//0th vertex contains nothing
		vertices.size() - 1, vertices.data() + 1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() - 1, indices.data(), GL_STATIC_DRAW);

	if (!inVAO) {

	}

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)0);
	glEnableVertexAttribArray(0);

	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)(sizeof(PxVec3)));
	glEnableVertexAttribArray(1);
}


void Mesh::draw(Shader& shader) {

}






physx::PxTriangleMeshGeometry Mesh::createPxGeometry(double scale) {
	using namespace physx;
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.stride = sizeof(Vertex);
	meshDesc.points.count = vertices.size();
	//I'm guessing the data field is the begin pos of the vertices, because I chose to start
	meshDesc.points.data = ((char*)vertices.data());

	/*
		triangles is the same as the EBO, but it's divided into groups of three.
	*/
	meshDesc.triangles.stride = 3 * sizeof(unsigned int);
	meshDesc.triangles.count = indices.size() / 3;
	meshDesc.triangles.data = indices.data();

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = P::gpCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (not status) {
		//Error
	}
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	auto mesh = P::gpPhysics->createTriangleMesh(readBuffer);
	PxMeshGeometryFlags flags; //No Flags
	PxTriangleMeshGeometry TriGeom(mesh, PxMeshScale(scale), flags);
	return TriGeom;
}