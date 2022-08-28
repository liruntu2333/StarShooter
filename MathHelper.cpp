#include "MathHelper.h"
using namespace DirectX;

XMFLOAT3 operator-(const XMFLOAT3& lft, const XMFLOAT3& rht) noexcept
{
	
	const XMVECTOR v1 = XMLoadFloat3(&lft);
	const XMVECTOR v2 = XMLoadFloat3(&rht);
	const XMVECTOR X = XMVectorSubtract(v1, v2);
	XMFLOAT3 R;
	XMStoreFloat3(&R, X);
	return R;
}

BezierCurveQuadratic::BezierCurveQuadratic(const std::array<DirectX::XMFLOAT3, 3>& controlPoints) : MControlPoints(controlPoints)
{
}

BezierCurveQuadratic::BezierCurveQuadratic(std::array<DirectX::XMFLOAT3, 3>&& controlPoints) : MControlPoints(controlPoints)
{
}

XMFLOAT3 BezierCurveQuadratic::GetPosition(const float t)
{
	XMFLOAT3 ret;
	const float oneMinusT = 1 - t;
	const XMVECTOR p0 = XMLoadFloat3(&MControlPoints[0]);
	const XMVECTOR p1 = XMLoadFloat3(&MControlPoints[1]);
	const XMVECTOR p2 = XMLoadFloat3(&MControlPoints[2]);
	XMVECTOR pos = p1 + oneMinusT * oneMinusT * (p0 - p1) + t * t * (p2 - p1);
	XMStoreFloat3(&ret, pos);
	return ret;
}

XMFLOAT3 BezierCurveQuadratic::GetDerivative(float t)
{
	XMFLOAT3 ret;
	const XMVECTOR p0 = XMLoadFloat3(&MControlPoints[0]);
	const XMVECTOR p1 = XMLoadFloat3(&MControlPoints[1]);
	const XMVECTOR p2 = XMLoadFloat3(&MControlPoints[2]);
	XMVECTOR der = 2.0f * (1 - t) * (p1 - p0) + 2.0f * t * (p2 - p1);
	XMStoreFloat3(&ret, der);
	return ret;
}

void BezierCurveQuadratic::SetControlPoint2(DirectX::XMFLOAT3 target)
{
	MControlPoints[2] = target;
}
