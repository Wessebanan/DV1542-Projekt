#pragma once
#include "TreeNode.h"
#include "Frustum.h"
#include <vector>


class QuadTree {
private:
	TreeNode rootNode;
	void FrustumCheck(std::vector<MeshObject*>* destinationList, Frustum* frustum, TreeNode* currentNode);



	
public:
	QuadTree(const std::vector<MeshObject*>* geometry, float rootX, float rootZ, float rootHalfWidth);
	~QuadTree();
	
	std::vector<MeshObject*> getVisibleObjects(Frustum* frustum);

};