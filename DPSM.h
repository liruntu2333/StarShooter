#pragma once
#include "wrl/client.h"
#include "main.h"

class DPSM	// Dual Paraboloid Shadow Mapping
{
public:

	DPSM(ID3D11Device* device, ID3D11DeviceContext* context, UINT width, UINT height, int iLight);
	DPSM(const DPSM&) = delete;
	~DPSM() = default;

	auto GetFrontDsv() const { return mDsvFront.Get(); }
	auto GetBackDsv() const { return mDsvBack.Get(); }
	auto GetLightIndex() const { return mLightIdx; }
	auto GetView() const { return mView; }
	void SetView(const XMMATRIX& view)
	{
		mView = view;
	}
	void SetViewPort() const
	{
		mImmediateContext->RSSetViewports(1, &mViewport);
	}
	void SetRect() const
	{
		mImmediateContext->RSSetScissorRects(1, &mScissorRect);
	}
	auto GetFrontSrv() const
	{
		return mSrvFront.Get();
	}
	auto GetBackSrv() const
	{
		return mSrvBack.Get();
	}

	static Microsoft::WRL::ComPtr<ID3D11VertexShader> sVs;
	static Microsoft::WRL::ComPtr<ID3D11PixelShader>  sPs;

private:
	void BuildDepthTexture();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSrvFront;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSrvBack;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	 mDsvFront;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	 mDsvBack;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>          mDepthTexFront;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>          mDepthTexBack;

	Microsoft::WRL::ComPtr<ID3D11Device>			 mD3DDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		 mImmediateContext;

	const DXGI_FORMAT mTexFormat = DXGI_FORMAT_R32_TYPELESS;
	const DXGI_FORMAT mSrvFormat = DXGI_FORMAT_R32_FLOAT;
	const DXGI_FORMAT mDsvFormat = DXGI_FORMAT_D32_FLOAT;

	const UINT mTexWidth;
	const UINT mTexHeight;

	const int mLightIdx;

	XMMATRIX mView = XMMatrixIdentity();

	D3D11_VIEWPORT mViewport{};
	D3D11_RECT mScissorRect{};
};

