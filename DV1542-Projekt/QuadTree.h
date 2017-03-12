#pragma once
#include "TreeNode.h"
#include <vector>

class QuadTree {
private:
	TreeNode rootNode;




	
public:
	QuadTree(const std::vector<MeshObject*>* geometry, float rootX, float rootZ, float rootHalfWidth);
	~QuadTree();
	
	std::vector<MeshObject*> getVisibleObjects(float FRUSTUMPLACEHOLDER);

};