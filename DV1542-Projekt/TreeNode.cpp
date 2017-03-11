#include "TreeNode.h"



TreeNode::TreeNode(float positionX, float positionZ, float width) {
	this->positionX = positionX;
	this->positionZ = positionZ;
	this->width = width;
	this->listHead = nullptr;
	this->currentObject = nullptr;
	
	int childWidth = this->width / 2;
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
}

TreeNode::~TreeNode() {
	this->currentObject = this->listHead;
	while (currentObject != nullptr) {
		ListObject* next = this->currentObject->nextNode;
		delete this->currentObject;
		this->currentObject = next;
	}
}

void TreeNode::AddObject(MeshObject * toAdd) {
	ListObject* newItem = new ListObject(toAdd, this->listHead);
	this->listHead = newItem;
	if (this->currentObject == nullptr) {
		this->currentObject = listHead;
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

MeshObject * TreeNode::getCurrentListObject()
{
	if (this->listHead != nullptr) { // If the list contains items
		if (this->currentObject == nullptr) {
			this->currentObject = this->listHead; // When list is being searched after the first search, reset the list
		}
		MeshObject* toReturn = this->currentObject->object;
		this->currentObject = this->currentObject->nextNode;
		return toReturn;
	}
	else {
		return nullptr;
	}
}


