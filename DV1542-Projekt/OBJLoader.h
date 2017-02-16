#pragma once
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>

struct Vertex
{
	float x, y, z;
	float r, g, b;
	float u, v;
};

using namespace DirectX;


bool loadOBJ(const char* filePath, std::vector <Vertex> &vertices) {
	std::vector <unsigned int> vertexIndices, texCoordIndices, normalIndices;
	std::vector <XMFLOAT3> tempVerts;
	std::vector <XMFLOAT2> tempTexCoords;
	std::vector <XMFLOAT3> tempNormals;

	FILE * file = fopen(filePath, "r"); // Open the file to be able to read from it
	if (file == NULL) {
		MessageBox(NULL, L"Could not open file while loading OBJ", NULL, MB_ICONEXCLAMATION);
		return false;
	}
	bool readingFile = true;
	while (readingFile) {
		char lineHeader[128];
		int result = fscanf(file, "%s", lineHeader);
		if (result == EOF) { // Terminate if end of file has been reached
			readingFile = false;
		}
		else {
			if (strcmp(lineHeader, "v") == 0) { // Current line to read is vertex info
				XMFLOAT3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				tempVerts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) { // Current line to read is texcoord info
				XMFLOAT2 texCoord;
				fscanf(file, "%f %f\n", &texCoord.x, &texCoord.y);
				tempTexCoords.push_back(texCoord);
			}
			else if (strcmp(lineHeader, "vn") == 0) { // Current line to read is normal info
				XMFLOAT3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				tempNormals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) { // Current line to read is face info
				unsigned int vertexIndex[3], texCoordIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &texCoordIndex[0], &normalIndex[0], 
					&vertexIndex[1], &texCoordIndex[1], &normalIndex[1], &vertexIndex[2], &texCoordIndex[2], &normalIndex[2]);
				if (matches != 9) {
					MessageBox(NULL, L"Face data in OBJ file does not match format of the OBJ reader!", NULL, MB_ICONEXCLAMATION);
					return false;
				}
				else {
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					texCoordIndices.push_back(texCoordIndex[0]);
					texCoordIndices.push_back(texCoordIndex[1]);
					texCoordIndices.push_back(texCoordIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

				}

			}
		}

	}
}