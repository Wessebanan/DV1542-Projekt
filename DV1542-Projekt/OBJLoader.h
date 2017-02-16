#pragma once
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>


using namespace DirectX;


bool loadOBJ(const char* filePath, std::vector <XMFLOAT3> &vertices, std::vector <XMFLOAT2> &texCoords, std::vector <XMFLOAT3> &normals) {
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
		}

	}
}