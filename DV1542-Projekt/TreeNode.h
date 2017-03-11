#pragma once
#include "MeshObject.h"

enum DIRECTION {
	NW = 0,
	NE = 1,
	SW = 2,
	SE = 3
};

struct ListObject {
	MeshObject* object;
	ListObject* nextNode;
	ListObject(MeshObject* newObject, ListObject* nextNode = nullptr) {
		this->object = newObject;
		this->nextNode = nextNode;
	}
};

class TreeNode {
private:
	float positionX, positionZ, width;
	ListObject* listHead;
	ListObject* currentObject;
	TreeNode* children[4];



public:
	TreeNode(float positionX, float positionZ, float width);
	~TreeNode();

	void AddObject(MeshObject* toAdd);

	TreeNode* getChild(DIRECTION direction);

	MeshObject* getCurrentListObject();
};