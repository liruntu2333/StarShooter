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

