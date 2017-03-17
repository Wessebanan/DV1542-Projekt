#include "Frustum.h"

float Frustum::distancePointToPlane(const Plane & plane, const XMVECTOR & point)
{
	//float c = -plane.Distance / plane.Normal.z;
	//XMVECTOR pointInPlane = { 0.0f, 0.0f, c };


	XMVECTOR Normal = { plane.Normal.x, plane.Normal.y, plane.Normal.z };
	//XMVECTOR dotVec = XMVector3Dot(Normal, point - pointInPlane);
	//return XMVectorGetX(dotVec);

	XMVECTOR d1Vec = XMVector3Dot(Normal, point);
	return XMVectorGetX(d1Vec) + plane.Distance;

}

Frustum::Frustum(const XMMATRIX & view, const XMMATRIX & projection) {
	UpdateFrustum(view, projection);
}

Frustum::~Frustum() {

}

void Frustum::UpdateFrustum(const XMMATRIX & view, const XMMATRIX & projection) {
	XMFLOAT4X4 M;
	XMStoreFloat4x4(&M, (view * projection));
	// Left plane
	this->Planes[0].Normal.x = -(M._14 + M._11);
	this->Planes[0].Normal.y = -(M._24 + M._21);
	this->Planes[0].Normal.z = -(M._34 + M._31);
	this->Planes[0].Distance = -(M._44 + M._41);

	// Right plane
	this->Planes[1].Normal.x = -(M._14 - M._11);
	this->Planes[1].Normal.y = -(M._24 - M._21);
	this->Planes[1].Normal.z = -(M._34 - M._31);
	this->Planes[1].Distance = -(M._44 - M._41);

	// Near plane
	this->Planes[2].Normal.x = -(M._14 + M._13);
	this->Planes[2].Normal.y = -(M._24 + M._23);
	this->Planes[2].Normal.z = -(M._34 + M._33);
	this->Planes[2].Distance = -(M._44 + M._43);

	// Far plane
	this->Planes[3].Normal.x = -(M._14 - M._13);
	this->Planes[3].Normal.y = -(M._24 - M._23);
	this->Planes[3].Normal.z = -(M._34 - M._33);
	this->Planes[3].Distance = -(M._44 - M._43);

	// Top plane
	this->Planes[4].Normal.x = -(M._14 - M._12);
	this->Planes[4].Normal.y = -(M._24 - M._22);
	this->Planes[4].Normal.z = -(M._34 - M._32);
	this->Planes[4].Distance = -(M._44 - M._42);

	// Bottom plane
	this->Planes[5].Normal.x = -(M._14 - M._12);
	this->Planes[5].Normal.y = -(M._24 - M._22);
	this->Planes[5].Normal.z = -(M._34 - M._32);
	this->Planes[5].Distance = -(M._44 - M._42);



	for (int i = 0; i < 6; i++) {
		float scalingFactor = 1.0f / sqrt(pow(this->Planes[i].Normal.x,2) + pow(this->Planes[i].Normal.y, 2) + pow(this->Planes[i].Normal.z, 2));
		this->Planes[i].Normal.x *= scalingFactor;
		this->Planes[i].Normal.y *= scalingFactor;
		this->Planes[i].Normal.z *= scalingFactor;
		this->Planes[i].Distance *= scalingFactor;
	}
}

bool Frustum::CheckAABA(const float & lowX, const float & highX, const float & lowZ, const float & highZ)
{
	XMVECTOR HighLow = { highX, 0, lowZ };
	XMVECTOR HighHigh = { highX, 0, highZ };
	XMVECTOR Center = { (lowX + highX) / 2.0f, 0, (lowZ + highZ) / 2.0f };
	XMVECTOR diagCentertoHH = HighHigh - Center;
	XMVECTOR diagCentertoHL = HighLow - Center;
	XMVECTOR diagonals[2] = { diagCentertoHH, diagCentertoHL };

	for (int i = 0; i < 4; i++) { // Check against left, right, near and far planes
		XMVECTOR planeNormal = { this->Planes[i].Normal.x, this->Planes[i].Normal.y, this->Planes[i].Normal.z };

		float bestDot = 0.0f;
		XMVECTOR bestDiagonal, N;
		bool dotNegative = false;

		for (int j = 0; j < 2; j++) {
			float newDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(planeNormal), XMVector3Normalize(diagonals[j])));
			float absDot = fabs(newDot);
			if (absDot > bestDot) {
				bestDot = absDot;
				bestDiagonal = diagonals[j];
				dotNegative = (newDot < 0.0f);
			}
		}
		if (!dotNegative) {
			N = Center - bestDiagonal;
		}
		else {
			N = Center + bestDiagonal;
		}

		if (distancePointToPlane(this->Planes[i], N) >= 0.0f) {
			// Box is completely outside of the plane
			return false;
		}

	}
	return true;
}
