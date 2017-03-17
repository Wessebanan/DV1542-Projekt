#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct Plane {
	XMFLOAT3 Normal;
	float Distance;
};

class Frustum {
private:
	Plane Planes[6]; // Left, Right, Near, Far, Top, Bottom
	float distancePointToPlane(const Plane &plane, const XMVECTOR &point);


public:

	Frustum(const XMMATRIX &view = XMMatrixIdentity() , const XMMATRIX &projection = XMMatrixIdentity() );
	~Frustum();

	void UpdateFrustum(const XMMATRIX &view, const XMMATRIX &projection);

	bool CheckAABA(const float &lowX, const float &highX, const float &lowZ, const float &highZ);



};