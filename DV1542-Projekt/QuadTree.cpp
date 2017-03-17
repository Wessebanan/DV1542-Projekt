#include "QuadTree.h"

void QuadTree::FrustumCheck(std::vector<MeshObject*>* destinationList, Frustum * frustum, TreeNode * currentNode) {
	if (currentNode != nullptr) {
		bool doesIntersect = frustum->CheckAABA(currentNode->getPositionX() - currentNode->getHalfWidth(), currentNode->getPositionX() + currentNode->getHalfWidth(), currentNode->getPositionZ() - currentNode->getHalfWidth(), currentNode->getPositionZ() + currentNode->getHalfWidth());
		if (doesIntersect) {
			MeshObject* toAdd = currentNode->getCurrentListObject();
			while (toAdd != nullptr) { // Add all objects in current node to list
				destinationList->push_back(toAdd);
				toAdd = currentNode->getCurrentListObject();
			}
			for (int i = 0; i < 4; i++) {
				FrustumCheck(destinationList, frustum, currentNode->getChild(NW));
				FrustumCheck(destinationList, frustum, currentNode->getChild(NE));
				FrustumCheck(destinationList, frustum, currentNode->getChild(SW));
				FrustumCheck(destinationList, frustum, currentNode->getChild(SE));

			}
		}
	}
}

QuadTree::QuadTree(const std::vector<MeshObject*>* geometry, float rootX, float rootZ, float rootHalfWidth) : rootNode(rootX, rootZ, rootHalfWidth) {
	int objectsToAdd = (int)geometry->size();
	for (int i = 0; i < objectsToAdd; i++) {
		MeshObject* currentObject = (*geometry)[i];
		this->rootNode.AddObject(currentObject);
	}
}

QuadTree::~QuadTree() {
}

std::vector<MeshObject*> QuadTree::getVisibleObjects(Frustum * frustum) {
	bool checking = true;
	TreeNode* currentNode = &this->rootNode;
	std::vector<MeshObject*> listOfVisibleObjects;
	FrustumCheck(&listOfVisibleObjects, frustum, currentNode);
	return listOfVisibleObjects;
}


