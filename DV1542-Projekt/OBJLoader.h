#pragma once
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>

struct Vertex
{
	float x, y, z;
	float norX, norY, norZ;
	float u, v;
};

struct Material {
	float Kdr, Kdg, Kdb;
	float Ksr, Ksg, Ksb;
	float Kar, Kag, Kab;
	float Ns;
	std::string textureFilePath;
};

enum TEX_COORD_TYPE {
	DIRECTX = 0,
	OPENGL = 1
};

using namespace DirectX;


Material* loadMTL(const char* filePath) {
	// This is a very basic implementation of a material file reader which reads ONE (1) material
	// from a .mtl file and uses it for the entire obj
	// Usage limited due to time constraints

	Material* newMat = new Material {0, 0, 0,
								     0, 0, 0,
					                 0, 0, 0,
					                 0,
					                 std::string("")};
	FILE* file;
	bool materialFound = false;
	fopen_s(&file, filePath, "r"); // Open file to read
	if (file == NULL) {
		MessageBox(NULL, L"Could not open file while loading MTL", NULL, MB_ICONEXCLAMATION);
	}
	bool readingFile = true;
	while (readingFile) {
		char lineHeader[128]; // Not good, but works
		int result = fscanf(file, "%s", &lineHeader);
		if (result == EOF) {
			readingFile = false; // End of file has been reached
		}
		else {
			if (strcmp(lineHeader, "newmtl") == 0) {
				if (!materialFound) {
					materialFound = true;
				}
				else {
					readingFile = false; // We have already read a material, ignore the rest of the file
				}
			}
			else if (strcmp(lineHeader, "Kd") == 0) {
				fscanf_s(file, "%f %f %f\n", newMat->Kdr, newMat->Kdg, newMat->Kdb);
			}
			else if (strcmp(lineHeader, "Ks") == 0) {
				fscanf_s(file, "%f %f %f\n", newMat->Ksr, newMat->Ksg, newMat->Ksb);
			}
			else if (strcmp(lineHeader, "Ka") == 0) {
				fscanf_s(file, "%f %f %f\n", newMat->Kar, newMat->Kag, newMat->Kab);
			}
			else if (strcmp(lineHeader, "Ns") == 0) {
				fscanf_s(file, "%f\n", newMat->Ns);
			}
			else if (strcmp(lineHeader, "map_Kd") == 0) {
				fscanf_s(file, "%s", newMat->textureFilePath);
			}
		}
	}
	return newMat;
}


bool loadOBJ(const char* filePath, std::vector <Vertex> &vertices, std::vector <unsigned int> &indices, TEX_COORD_TYPE texType = DIRECTX) {
	// This function reads obj files of format
	// v 1 1 1
	// vt 1 1 
	// vn 1 1 1
	// f 1/1/1 1/1/1 1/1/1


	std::vector <unsigned int> vertexIndices, texCoordIndices, normalIndices;
	std::vector <XMFLOAT3> tempVerts;
	std::vector <XMFLOAT2> tempTexCoords;
	std::vector <XMFLOAT3> tempNormals;

	FILE* file;
	fopen_s(&file, filePath, "r"); // Open the file to be able to read from it
	if (file == NULL) {
		MessageBox(NULL, L"Could not open file while loading OBJ", NULL, MB_ICONEXCLAMATION);
		return false;
	}
	bool readingFile = true;
	while (readingFile) {
		char lineHeader[128];
		int result = fscanf(file, "%s", &lineHeader);
		if (result == EOF) { // Terminate if end of file has been reached
			readingFile = false;
		}
		else {
			if (strcmp(lineHeader, "v") == 0) { // Current line to read is vertex info
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				tempVerts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) { // Current line to read is texcoord info
				XMFLOAT2 texCoord;
				fscanf_s(file, "%f %f\n", &texCoord.x, &texCoord.y);
				tempTexCoords.push_back(texCoord);
			}
			else if (strcmp(lineHeader, "vn") == 0) { // Current line to read is normal info
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				tempNormals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) { // Current line to read is face info
				unsigned int vertexIndex[3], texCoordIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &texCoordIndex[0], &normalIndex[0], 
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
	for (int i = 0; i < vertexIndices.size(); i++) {
		Vertex newVert;
		if (texType == DIRECTX) {
			newVert = { tempVerts[vertexIndices[i] - 1].x, tempVerts[vertexIndices[i] - 1].y, tempVerts[vertexIndices[i] - 1].z,
								tempNormals[normalIndices[i] - 1].x, tempNormals[normalIndices[i] - 1].y, tempNormals[normalIndices[i] - 1].z,
								tempTexCoords[texCoordIndices[i] - 1].x, tempTexCoords[texCoordIndices[i] - 1].y };
		}
		else {
			newVert = { tempVerts[vertexIndices[i] - 1].x, tempVerts[vertexIndices[i] - 1].y, tempVerts[vertexIndices[i] - 1].z,
				tempNormals[normalIndices[i] - 1].x, tempNormals[normalIndices[i] - 1].y, tempNormals[normalIndices[i] - 1].z,
				tempTexCoords[texCoordIndices[i] - 1].x, 1 - tempTexCoords[texCoordIndices[i] - 1].y };
		}
		vertices.push_back(newVert);
		indices.push_back(i);
	}
	return true;
}