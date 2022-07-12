/**
 * \file   MathHelper.h
 * \brief  Math helper functions to do lerp, slerp, clamp, etc
 * 
 * \author LiRuntu
 * \date   July 2022
 **/
#pragma once

#include <DirectXMath.h>

class MathHelper
{
public:
	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	//static DirectX::XMVECTOR XM_CALLCONV ToXMVECTOR(const DirectX::XMFLOAT3& vec)
	//{
	//	return XMLoadFloat3(&vec);
	//}

	//static DirectX::XMVECTOR XM_CALLCONV ToXMVECTOR(const DirectX::XMFLOAT4& vec)
	//{
	//	return XMLoadFloat4(&vec);
	//}
};
