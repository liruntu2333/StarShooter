#include "DPSM.h"

Microsoft::WRL::ComPtr<ID3D11VertexShader> DPSM::sVs = nullptr;
Microsoft::WRL::ComPtr<ID3D11PixelShader>  DPSM::sPs = nullptr;

DPSM::DPSM(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height, int iLight)
	: mD3DDevice(device), mImmediateContext(context), mTexWidth(width), mTexHeight(height), mLightIdx(iLight)
{
	if (sVs == nullptr)
	{
		// compile VS
		ID3DBlob* pErrorBlob;
		ID3DBlob* pVsBlob = nullptr;
		DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)// && defined(DEBUG_SHADER)
		shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = D3DX11CompileFromFile("dpsm.hlsl", nullptr, nullptr, 
			"ShadowVS", "vs_5_0", shFlag, 0, nullptr,
			&pVsBlob, &pErrorBlob, nullptr);

		if (FAILED(hr))
			MessageBox(nullptr, static_cast<char*>(pErrorBlob->GetBufferPointer()), 
				"VS", MB_OK | MB_ICONERROR);

		mD3DDevice->CreateVertexShader(pVsBlob->GetBufferPointer(), pVsBlob->GetBufferSize(), nullptr, &DPSM::sVs);

		//pErrorBlob->Release();
		pVsBlob->Release();
	}

	if (sPs == nullptr)
	{
		// compile PS
		ID3DBlob* pErrorBlob;
		ID3DBlob* pPsBlob = nullptr;
		DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG) //&& defined(DEBUG_SHADER)
		shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = D3DX11CompileFromFile("dpsm.hlsl", nullptr, nullptr, 
			"ShadowPS", "ps_5_0", shFlag, 0, nullptr, 
			&pPsBlob, &pErrorBlob, nullptr);

		if (FAILED(hr))
			MessageBox(nullptr, static_cast<char*>(pErrorBlob->GetBufferPointer()),
			           "PS", MB_OK | MB_ICONERROR);

		mD3DDevice->CreatePixelShader(pPsBlob->GetBufferPointer(), pPsBlob->GetBufferSize(), nullptr, &DPSM::sPs);

		pPsBlob->Release();
		//pErrorBlob->Release();
	}

	BuildDepthTexture();

	mViewport = {
		0.0f, 0.0f,
		(float)mTexWidth, (float)mTexHeight,
		0.0f, 1.0f
	};

	mScissorRect = { 0, 0, static_cast<int>(mTexWidth), static_cast<int>(mTexHeight) };
}

void DPSM::BuildDepthTexture()
{
	const auto texDesc = CD3D11_TEXTURE2D_DESC(mTexFormat, mTexWidth, mTexHeight, 1, 1,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);
	
	mD3DDevice->CreateTexture2D(&texDesc, nullptr, mDepthTexFront.GetAddressOf());
	mD3DDevice->CreateTexture2D(&texDesc, nullptr, mDepthTexBack.GetAddressOf());

	auto srvDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURE2D, mSrvFormat);
	mD3DDevice->CreateShaderResourceView(mDepthTexFront.Get(), &srvDesc, mSrvFront.GetAddressOf());
	mD3DDevice->CreateShaderResourceView(mDepthTexBack.Get(), &srvDesc, mSrvBack.GetAddressOf());

	auto dsvFront = CD3D11_DEPTH_STENCIL_VIEW_DESC(mDepthTexFront.Get(),
		D3D11_DSV_DIMENSION_TEXTURE2D, mDsvFormat);
	auto dsvBack = CD3D11_DEPTH_STENCIL_VIEW_DESC(mDepthTexBack.Get(),
		D3D11_DSV_DIMENSION_TEXTURE2D, mDsvFormat);
	mD3DDevice->CreateDepthStencilView(mDepthTexFront.Get(), &dsvFront, mDsvFront.GetAddressOf());
	mD3DDevice->CreateDepthStencilView(mDepthTexBack.Get(), &dsvBack, mDsvBack.GetAddressOf());
}
