#include "TreeNode.h"



void TreeNode::AddObjectHere(MeshObject * toAdd) {
	ListObject* newItem = new ListObject(toAdd, this->listHead);
	this->listHead = newItem;
	this->currentObject = listHead;
}

TreeNode::TreeNode(float positionX, float positionZ, float halfWidth) {
	this->positionX = positionX;
	this->positionZ = positionZ;
	this->halfWidth = halfWidth;
	this->listHead = nullptr;
	this->currentObject = nullptr;
	
	float childWidth = this->halfWidth / 2.0f;
	if ((childWidth) >= 32) {
		// NorthWest child
		this->children[0] = new TreeNode(this->positionX - childWidth, this->positionZ + childWidth, childWidth);
		// NorthEast child
		this->children[1] = new TreeNode(this->positionX + childWidth, this->positionZ + childWidth, childWidth);
		// SouthWest child
		this->children[2] = new TreeNode(this->positionX - childWidth, this->positionZ - childWidth, childWidth);
		// SouthEast child
		this->children[3] = new TreeNode(this->positionX + childWidth, this->positionZ - childWidth, childWidth);
	}
	else {
		for (int i = 0; i < 4; i++) {
			this->children[i] = nullptr;
		}
	}
}

TreeNode::~TreeNode() {
	this->currentObject = this->listHead;
	while (currentObject != nullptr) {
		ListObject* next = this->currentObject->nextNode;
		delete this->currentObject;
		this->currentObject = next;
	}
	for (int i = 0; i < 4; i++) {
		if (this->children[i] != nullptr)
			delete this->children[i];
	}
}

void TreeNode::AddObject(MeshObject * toAdd) {
	XMFLOAT4 objectBVs = toAdd->getBoundingValues();
	// objectBVs
	// x: lowX  y: highX  z: lowZ  w: highZ


	if (this->children[0] != nullptr) { // If this is not a leaf node

		if (objectBVs.x > this->positionX) { // Object is completely in the East half
			if (objectBVs.z > this->positionZ) { // Object is in North-East quadrant
				this->children[1]->AddObject(toAdd);
			}
			else if (objectBVs.w < this->positionZ) { // Object is in South-East quadrant
				this->children[3]->AddObject(toAdd);
			}
			else {
				// Object intersects quadrants, add it here
				this->AddObjectHere(toAdd);
			}

		}
		else if (objectBVs.y < this->positionX) { // Object is completely in the West half
			if (objectBVs.z > this->positionZ) { // Object is in North-West quadrant
				this->children[0]->AddObject(toAdd);
			}
			else if (objectBVs.w < this->positionZ) { // Object is in South-West quadrant
				this->children[2]->AddObject(toAdd);
			}
			else {
				// Object intersects quadrants, add it here
				this->AddObjectHere(toAdd);
			}
		}
		else {
			// Object intersects quadrants, add it here
			this->AddObjectHere(toAdd);
		}
	}

	else { // This is a leaf node, add the object here
		this->AddObjectHere(toAdd);
	}
}



TreeNode * TreeNode::getChild(DIRECTION direction)
{
	switch (direction)
	{
	case NW:
		return this->children[0];
		break;
	case NE:
		return this->children[1];
		break;
	case SW:
		return this->children[2];
		break;
	case SE:
		return this->children[3];
		break;
	default:
		return nullptr;
		break;
	}
}

float TreeNode::getPositionX()
{
	return this->positionX;
}

float TreeNode::getPositionZ()
{
	return this->positionZ;
}

float TreeNode::getHalfWidth()
{
	return this->halfWidth;
}

MeshObject * TreeNode::getCurrentListObject()
{
	if (this->listHead != nullptr) { // If the list contains items
		MeshObject* toReturn = nullptr;
		if (this->currentObject == nullptr) {
			this->currentObject = this->listHead; // When list is being searched after the first search, reset the list
		}
		else {
			toReturn = this->currentObject->object;
			this->currentObject = this->currentObject->nextNode;
		}
		return toReturn;
	}
	else {
		return nullptr;
	}
}


