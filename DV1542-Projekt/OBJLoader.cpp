#include "OBJLoader.h"

Material* loadMTL(const char* filePath) {
	// This is a very basic implementation of a material file reader which reads ONE (1) material
	// from a .mtl file and uses it for the entire obj
	// Usage limited due to time constraints

	Material* newMat = new Material{ 0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0,
		"\0" };
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
				fscanf_s(file, "%f %f %f\n", &newMat->Kdr, &newMat->Kdg, &newMat->Kdb);
			}
			else if (strcmp(lineHeader, "Ks") == 0) {
				fscanf_s(file, "%f %f %f\n", &newMat->Ksr, &newMat->Ksg, &newMat->Ksb);
			}
			else if (strcmp(lineHeader, "Ka") == 0) {
				fscanf_s(file, "%f %f %f\n", &newMat->Kar, &newMat->Kag, &newMat->Kab);
			}
			else if (strcmp(lineHeader, "Ns") == 0) {
				fscanf_s(file, "%f\n", &newMat->Ns);
			}
			else if (strcmp(lineHeader, "map_Kd") == 0) {
				fscanf(file, "%s\n", &newMat->textureFilePath); // fscanf_s does not like to do this one, god damn i hate these functions
			}
		}
	}
	return newMat;
}

bool loadOBJ(const char* filePath, std::vector <Vertex> &vertices, std::vector <unsigned int> &indices, Material* &objectMaterial, XMFLOAT3* &boundingValues, TEX_COORD_TYPE texType) {
	// This function reads obj files of format
	// v 1 1 1
	// vt 1 1 
	// vn 1 1 1
	// f 1/1/1 1/1/1 1/1/1


	std::vector <unsigned int> vertexIndices, texCoordIndices, normalIndices;
	std::vector <XMFLOAT3> tempVerts;
	std::vector <XMFLOAT2> tempTexCoords;
	std::vector <XMFLOAT3> tempNormals;

	float minX = FLT_MAX, minZ = FLT_MAX;
	float maxX = -FLT_MAX, maxZ = -FLT_MAX;

	FILE* file;
	fopen_s(&file, filePath, "r"); // Open the file to be able to read from it
	if (file == NULL) {
		MessageBox(NULL, L"Could not open file while loading OBJ", NULL, MB_ICONEXCLAMATION);
		return false;
	}
	bool readingFile = true;
	while (readingFile) {
		char lineHeader[128]; // Not good, but works
		int result = fscanf(file, "%s", &lineHeader);
		if (result == EOF) { // Terminate if end of file has been reached
			readingFile = false;
		}
		else {
			if (strcmp(lineHeader, "mtllib") == 0) {
				char filePathMtl[128];
				fscanf(file, "%s\n", &filePathMtl);
				objectMaterial = loadMTL(filePathMtl);
			}
			else if (strcmp(lineHeader, "v") == 0) { // Current line to read is vertex info
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				// Bounding values calculated
				if (vertex.x < minX)
					minX = vertex.x;
				else if (vertex.x > maxX)
					maxX = vertex.x;

				if (vertex.z < minZ)
					minZ = vertex.z;
				else if (vertex.z > maxZ)
					maxZ = vertex.z;

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
	if (minX < (FLT_MAX - 1) && boundingValues != nullptr) { // Something has been found
		float offsetX = (maxX + minX) / 2; // The offset of the bounding area from the origin of the mesh
		float offsetZ = (maxZ + minZ) / 2;

		float xWidth = maxX - minX;
		float zWidth = maxZ - minZ;
		float halfWidth = (xWidth > zWidth) ? (xWidth / 2) : (zWidth / 2); // Choose the largest width
		boundingValues->x = offsetX;
		boundingValues->y = offsetZ;
		boundingValues->z = halfWidth;

	}
	return true;
}