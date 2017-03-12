#include "QuadTree.h"

QuadTree::QuadTree(const std::vector<MeshObject*>* geometry, float rootX, float rootZ, float rootHalfWidth) : rootNode(rootX, rootZ, rootHalfWidth) {
	int objectsToAdd = (int)geometry->size();
	for (int i = 0; i < objectsToAdd; i++) {
		MeshObject* currentObject = (*geometry)[i];
		this->rootNode.AddObject(currentObject);
	}
}
