#pragma once

#include <array>
#include <ctime>
#include <DirectXMath.h>
#include <vector>

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

	static float RandF()
	{
		return static_cast<float>(rand()) / RAND_MAX;
	}

};

class BezierCurve
{
public:
	BezierCurve() = default;
	BezierCurve(const BezierCurve&) = default;
	BezierCurve(BezierCurve&&) = default;
	BezierCurve& operator=(const BezierCurve&) = default;
	BezierCurve& operator=(BezierCurve&&) = default;
	virtual ~BezierCurve() = default;

	virtual DirectX::XMFLOAT3 GetPosition(float t) = 0;
	virtual DirectX::XMFLOAT3 GetDerivative(float t) = 0;
	virtual DirectX::XMFLOAT3 GetNormalizedDerivative(float t) = 0;
	virtual void SetControlPointsBack(DirectX::XMFLOAT3 target) = 0;
};

class BezierCurveQuadratic : BezierCurve
{
public:
	explicit BezierCurveQuadratic(const std::array<DirectX::XMFLOAT3, 3>& controlPoints);
	explicit BezierCurveQuadratic(std::array<DirectX::XMFLOAT3, 3>&& controlPoints);
	BezierCurveQuadratic(const BezierCurveQuadratic&) = default;
	BezierCurveQuadratic(BezierCurveQuadratic&&) = default;
	BezierCurveQuadratic& operator=(const BezierCurveQuadratic&) = default;
	BezierCurveQuadratic& operator=(BezierCurveQuadratic&&) = default;
	~BezierCurveQuadratic() override = default;

	DirectX::XMFLOAT3 GetPosition(float t) override;
	DirectX::XMFLOAT3 GetDerivative(float t) override;
	DirectX::XMFLOAT3 GetNormalizedDerivative(float t) override;
	void SetControlPointsBack(DirectX::XMFLOAT3 target) override;

private:
	std::array<DirectX::XMFLOAT3, 3> MControlPoints{};
};

DirectX::XMFLOAT3 operator- (const DirectX::XMFLOAT3& lft, const DirectX::XMFLOAT3& rht) noexcept;