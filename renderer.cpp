#include "main.h"
#include "renderer.h"

#include <ostream>
#include <string>
#include <vector>

#include "wrl/client.h"
#include <comdef.h>
#include <iostream>

#define DEBUG_DISP_TEXTOUT

struct MATERIAL_CBUFFER
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];				 
};

struct LIGHT_FLAGS
{
	int			Type;		 
	int         OnOff;		
	int			Dummy[2];
};

struct LIGHT_CBUFFER
{
	XMFLOAT4	Direction[LIGHT_MAX];	 
	XMFLOAT4	Position[LIGHT_MAX];	 
	XMFLOAT4	Diffuse[LIGHT_MAX];		 
	XMFLOAT4	Ambient[LIGHT_MAX];		 
	XMFLOAT4	Attenuation[LIGHT_MAX];	 
	LIGHT_FLAGS	Flags[LIGHT_MAX];		 
	int			Enable;
	int			Dummy[3];
};

struct FOG_CBUFFER
{
	XMFLOAT4	Fog;					 
	XMFLOAT4	FogColor;				 
	int			Enable;					 
	float		Dummy[3];				 
};

struct FUCHI
{
	int			fuchi;
	int			fill[3];
};

struct LightViewsBuffer
{
	XMMATRIX Views[5];
};

static void SetLightBuffer(void);
void InitSkyBox();
void CreateSphere(int LatLines, int LongLines);

namespace 
{
	const std::string g_SkyBoxPath = "data/TEXTURE/polluted.dds";
	D3D_FEATURE_LEVEL       g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	ID3D11Device* g_D3DDevice = nullptr;
	ID3D11DeviceContext* g_ImmediateContext = nullptr;
	IDXGISwapChain* g_SwapChain = nullptr;
	ID3D11RenderTargetView* g_RenderTargetView = nullptr;
	ID3D11DepthStencilView* g_DepthStencilView = nullptr;

	ID3D11VertexShader* g_VertexShader = nullptr;
	ID3D11PixelShader* g_PixelShader   = nullptr;
	ID3D11InputLayout* g_VertexLayout  = nullptr;
	ID3D11Buffer* g_WorldBuffer        = nullptr;
	ID3D11Buffer* g_ViewBuffer         = nullptr;
	ID3D11Buffer* g_ProjectionBuffer   = nullptr;
	ID3D11Buffer* g_MaterialBuffer     = nullptr;
	ID3D11Buffer* g_LightBuffer        = nullptr;
	ID3D11Buffer* g_FogBuffer          = nullptr;
	ID3D11Buffer* g_LightIndexBuffer        = nullptr;
	ID3D11Buffer* g_CameraBuffer       = nullptr;
	ID3D11Buffer* g_LightViewBuffer	   = nullptr;

	ID3D11DepthStencilState* g_DepthStateEnable;
	ID3D11DepthStencilState* g_DepthStateDisable;

	ID3D11BlendState* g_BlendStateNone;
	ID3D11BlendState* g_BlendStateAlphaBlend;
	ID3D11BlendState* g_BlendStateAdd;
	ID3D11BlendState* g_BlendStateSubtract;
	BLEND_MODE				g_BlendStateParam;

	ID3D11RasterizerState* g_RasterStateCullOff;
	ID3D11RasterizerState* g_RasterStateCullCW;
	ID3D11RasterizerState* g_RasterStateCullCCW;
	ID3D11RasterizerState* g_RasterStateSingleSample;

	MATERIAL_CBUFFER	g_Material;
	LIGHT_CBUFFER	g_Light;
	FOG_CBUFFER		g_Fog;

	FUCHI			g_Fuchi;

	namespace SkyBox
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>             g_VertBuff;
		Microsoft::WRL::ComPtr<ID3D11Buffer>             g_IdxBuff;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>       g_VertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>        g_PixelShader;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_ShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>          g_SkyBoxTex;

		int g_NumSphereVertices;
		int g_NumSphereFaces;
		XMMATRIX g_SphereWorld = XMMatrixIdentity();
	}

	D3D11_VIEWPORT g_ViewPort;
}

ID3D11Device* GetDevice(void)
{
	return g_D3DDevice;
}

ID3D11DeviceContext* GetDeviceContext(void)
{
	return g_ImmediateContext;
}

void ReturnToMainPass()
{
	g_ImmediateContext->RSSetViewports(1, &g_ViewPort);
	//g_ImmediateContext->RSSetScissorRects(1, )
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->VSSetShader(g_VertexShader, nullptr, 0);
	g_ImmediateContext->PSSetShader(g_PixelShader, nullptr, 0);
	SetCullingMode(CULL_MODE_BACK);
}

void SetDepthEnable(BOOL Enable)
{
	if (Enable)
		g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	else
		g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
}

void SetBlendState(BLEND_MODE bm)
{
	g_BlendStateParam = bm;

	const float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	switch (g_BlendStateParam)
	{
	case BLEND_MODE_NONE:
		g_ImmediateContext->OMSetBlendState(g_BlendStateNone, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_ALPHABLEND:
		g_ImmediateContext->OMSetBlendState(g_BlendStateAlphaBlend, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_ADD:
		g_ImmediateContext->OMSetBlendState(g_BlendStateAdd, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_SUBTRACT:
		g_ImmediateContext->OMSetBlendState(g_BlendStateSubtract, blendFactor, 0xffffffff);
		break;
	}
}

void SetCullingMode(CULL_MODE cm)
{
	switch (cm)
	{
	case CULL_MODE_NONE:
		g_ImmediateContext->RSSetState(g_RasterStateCullOff);
		break;
	case CULL_MODE_FRONT:
		g_ImmediateContext->RSSetState(g_RasterStateCullCW);
		break;
	case CULL_MODE_BACK:
		g_ImmediateContext->RSSetState(g_RasterStateCullCCW);
		break;

	default:
			g_ImmediateContext->RSSetState(g_RasterStateSingleSample);
		break;
	}
}

void SetAlphaTestEnable(BOOL flag)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	if (flag)
		blendDesc.AlphaToCoverageEnable = TRUE;
	else
		blendDesc.AlphaToCoverageEnable = FALSE;

	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	switch (g_BlendStateParam)
	{
	case BLEND_MODE_NONE:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_ALPHABLEND:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_ADD:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_SUBTRACT:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}

	const float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* blendState = nullptr;
	g_D3DDevice->CreateBlendState(&blendDesc, &blendState);
	g_ImmediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);

	if (blendState != nullptr)
		blendState->Release();
}

void SetWorldViewProjection2D(void)
{
	XMMATRIX world{};
	world = XMMatrixTranspose(XMMatrixIdentity());
	GetDeviceContext()->UpdateSubresource(g_WorldBuffer, 0, nullptr, &world, 0, 0);

	XMMATRIX view{};
	view = XMMatrixTranspose(XMMatrixIdentity());
	GetDeviceContext()->UpdateSubresource(g_ViewBuffer, 0, nullptr, &view, 0, 0);

	XMMATRIX worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	worldViewProjection = XMMatrixTranspose(worldViewProjection);
	GetDeviceContext()->UpdateSubresource(g_ProjectionBuffer, 0, nullptr, &worldViewProjection, 0, 0);
}

void SetWorldMatrix(XMMATRIX* WorldMatrix)
{
	XMMATRIX world = *WorldMatrix;
	world = XMMatrixTranspose(world);

	GetDeviceContext()->UpdateSubresource(g_WorldBuffer, 0, nullptr, &world, 0, 0);
}

void SetViewMatrix(XMMATRIX* ViewMatrix)
{
	XMMATRIX view = *ViewMatrix;
	view = XMMatrixTranspose(view);

	GetDeviceContext()->UpdateSubresource(g_ViewBuffer, 0, nullptr, &view, 0, 0);
}

void SetProjectionMatrix(XMMATRIX* ProjectionMatrix)
{
	XMMATRIX projection = *ProjectionMatrix;
	projection = XMMatrixTranspose(projection);

	GetDeviceContext()->UpdateSubresource(g_ProjectionBuffer, 0, nullptr, &projection, 0, 0);
}

void SetMaterial(MATERIAL material)
{
	g_Material.Diffuse = material.Diffuse;
	g_Material.Ambient = material.Ambient;
	g_Material.Specular = material.Specular;
	g_Material.Emission = material.Emission;
	g_Material.Shininess = material.Shininess;
	g_Material.noTexSampling = material.noTexSampling;

	GetDeviceContext()->UpdateSubresource(g_MaterialBuffer, 0, nullptr, &g_Material, 0, 0);
}

void SetLightBuffer(void)
{
	GetDeviceContext()->UpdateSubresource(g_LightBuffer, 0, nullptr, &g_Light, 0, 0);
}

void SetLightEnable(BOOL flag)
{
	g_Light.Enable = flag;

	SetLightBuffer();
}

void SetLight(int index, LIGHT* pLight)
{
	g_Light.Position[index] = XMFLOAT4(pLight->Position.x, pLight->Position.y, pLight->Position.z, 0.0f);
	g_Light.Direction[index] = XMFLOAT4(pLight->Direction.x, pLight->Direction.y, pLight->Direction.z, 0.0f);
	g_Light.Diffuse[index] = pLight->Diffuse;
	g_Light.Ambient[index] = pLight->Ambient;
	g_Light.Flags[index].Type = pLight->Type;
	g_Light.Flags[index].OnOff = pLight->Enable;
	g_Light.Attenuation[index].x = pLight->Attenuation;

	SetLightBuffer();
}

void SetFogBuffer(void)
{
	GetDeviceContext()->UpdateSubresource(g_FogBuffer, 0, nullptr, &g_Fog, 0, 0);
}

void SetFogEnable(BOOL flag)
{
	g_Fog.Enable = flag;

	SetFogBuffer();
}

void SetFog(FOG* pFog)
{
	g_Fog.Fog.x = pFog->FogStart;
	g_Fog.Fog.y = pFog->FogEnd;
	g_Fog.FogColor = pFog->FogColor;

	SetFogBuffer();
}

void SetLightIndexBuff(int flag)
{
	g_Fuchi.fuchi = flag;
	GetDeviceContext()->UpdateSubresource(g_LightIndexBuffer, 0, nullptr, &g_Fuchi, 0, 0);
}

void SetLightViews(std::vector<XMMATRIX> views)
{
	assert(views.size() == 5);
	for (auto & view : views)
	{
		view = XMMatrixTranspose(view);
	}
	GetDeviceContext()->UpdateSubresource(g_LightViewBuffer, 0, nullptr, views.data(), 0, 0);
}

void SetShaderCamera(XMFLOAT3 pos)
{
	const auto tmp = XMFLOAT4(pos.x, pos.y, pos.z, 0.0f);

	GetDeviceContext()->UpdateSubresource(g_CameraBuffer, 0, nullptr, &tmp, 0, 0);
}

void DrawSkyBox()
{
	SetWorldMatrix(&SkyBox::g_SphereWorld);
	SetCullingMode(CULL_MODE_NONE);
	SetLightEnable(FALSE);

	g_ImmediateContext->IASetIndexBuffer(SkyBox::g_IdxBuff.Get(), DXGI_FORMAT_R32_UINT, 0);

	constexpr UINT stride = sizeof(VERTEX_3D);
	constexpr UINT offset = 0;
	g_ImmediateContext->IASetVertexBuffers(0, 1, SkyBox::g_VertBuff.GetAddressOf(), &stride, &offset);
	g_ImmediateContext->VSSetShader(SkyBox::g_VertexShader.Get(), nullptr, 0);
	g_ImmediateContext->PSSetShader(SkyBox::g_PixelShader.Get(), nullptr, 0);
	g_ImmediateContext->PSSetShaderResources(1, 1, SkyBox::g_ShaderResourceView.GetAddressOf());

	g_ImmediateContext->DrawIndexed(SkyBox::g_NumSphereFaces * 3, 0, 0);

	g_ImmediateContext->VSSetShader(g_VertexShader, nullptr, 0);
	g_ImmediateContext->PSSetShader(g_PixelShader, nullptr, 0);
	SetCullingMode(CULL_MODE_BACK);
	SetLightEnable(TRUE);
}

void UpdateSkyBox(XMFLOAT3 cameraPos)
{
	SkyBox::g_SphereWorld = XMMatrixIdentity();

	const auto scale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	const auto translation = XMMatrixTranslation(cameraPos.x, cameraPos.y, cameraPos.z);

	SkyBox::g_SphereWorld = SkyBox::g_SphereWorld * scale * translation;
}

HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	DWORD deviceFlags = 0;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 8;
#ifdef _DEBUG
	sd.SampleDesc.Count = 1;
#endif
	sd.SampleDesc.Quality = 0;
	sd.Windowed = bWindow;

#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
	deviceFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif

	hr = D3D11CreateDeviceAndSwapChain(nullptr,
	                                   D3D_DRIVER_TYPE_HARDWARE,
	                                   nullptr,
	                                   deviceFlags,
	                                   nullptr,
	                                   0,
	                                   D3D11_SDK_VERSION,
	                                   &sd,
	                                   &g_SwapChain,
	                                   &g_D3DDevice,
	                                   &g_FeatureLevel,
	                                   &g_ImmediateContext);
	if (FAILED(hr))
		return hr;

#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	hr = g_SwapChain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE);      
	if (FAILED(hr))
		return hr;
#endif

	ID3D11Texture2D* pBackBuffer = nullptr;
	g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_D3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_RenderTargetView);
	pBackBuffer->Release();

	ID3D11Texture2D* depthTexture = nullptr;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = sd.BufferDesc.Width;
	td.Height = sd.BufferDesc.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc = sd.SampleDesc;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	g_D3DDevice->CreateTexture2D(&td, nullptr, &depthTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
#ifdef _DEBUG
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
#endif
	dsvd.Flags = 0;
	g_D3DDevice->CreateDepthStencilView(depthTexture, &dsvd, &g_DepthStencilView);

	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(SCREEN_WIDTH);
	vp.Height = static_cast<FLOAT>(SCREEN_HEIGHT);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_ImmediateContext->RSSetViewports(1, &vp);
	g_ViewPort = vp;

	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = true;
	g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullOff);

	rd.CullMode = D3D11_CULL_FRONT;
	g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullCW);

	rd.CullMode = D3D11_CULL_BACK;
	g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullCCW);

	rd.MultisampleEnable = FALSE;
	g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateSingleSample);

	SetCullingMode(CULL_MODE_BACK);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateAlphaBlend);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateNone);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateAdd);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateSubtract);

	SetBlendState(BLEND_MODE_ALPHABLEND);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateEnable);

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateDisable);

	SetDepthEnable(TRUE);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState = nullptr;
	g_D3DDevice->CreateSamplerState(&samplerDesc, &samplerState);
	g_ImmediateContext->PSSetSamplers(0, 1, &samplerState);

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerState = nullptr;
	g_D3DDevice->CreateSamplerState(&samplerDesc, &samplerState);
	g_ImmediateContext->PSSetSamplers(1, 1, &samplerState);

	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	float boarderCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
	memcpy(&samplerDesc.BorderColor, &boarderCol, sizeof(float) * 4);

	samplerState = nullptr;
	g_D3DDevice->CreateSamplerState(&samplerDesc, &samplerState);
	g_ImmediateContext->PSSetSamplers(2, 1, &samplerState);

	ID3DBlob* pErrorBlob;
	ID3DBlob* pVSBlob = nullptr;
	DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG) && defined(DEBUG_SHADER)
	shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	hr = D3DX11CompileFromFile("shader.hlsl", nullptr, nullptr, "VertexShaderPolygon", "vs_5_0", shFlag, 0, nullptr, &pVSBlob, &pErrorBlob, nullptr);
	if (FAILED(hr))
	{
		MessageBox(nullptr, static_cast<char*>(pErrorBlob->GetBufferPointer()), "VS", MB_OK | MB_ICONERROR);
	}

	g_D3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_VertexShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	g_D3DDevice->CreateInputLayout(layout,
		numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&g_VertexLayout);

	pVSBlob->Release();

	ID3DBlob* pPSBlob = nullptr;
	hr = D3DX11CompileFromFile("shader.hlsl", nullptr, nullptr, 
		"PixelShaderPolygon", "ps_5_0", shFlag, 0, nullptr, 
		&pPSBlob, &pErrorBlob, nullptr);
	if (FAILED(hr))
	{
		MessageBox(nullptr, static_cast<char*>(pErrorBlob->GetBufferPointer()), 
			"PS", MB_OK | MB_ICONERROR);
	}

	g_D3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_PixelShader);

	pPSBlob->Release();

	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(XMMATRIX);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_WorldBuffer);
	g_ImmediateContext->VSSetConstantBuffers(0, 1, &g_WorldBuffer);
	g_ImmediateContext->PSSetConstantBuffers(0, 1, &g_WorldBuffer);

	g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_ViewBuffer);
	g_ImmediateContext->VSSetConstantBuffers(1, 1, &g_ViewBuffer);
	g_ImmediateContext->PSSetConstantBuffers(1, 1, &g_ViewBuffer);

	g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_ProjectionBuffer);
	g_ImmediateContext->VSSetConstantBuffers(2, 1, &g_ProjectionBuffer);
	g_ImmediateContext->PSSetConstantBuffers(2, 1, &g_ProjectionBuffer);

	hBufferDesc.ByteWidth = sizeof(MATERIAL_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_MaterialBuffer);
	g_ImmediateContext->VSSetConstantBuffers(3, 1, &g_MaterialBuffer);
	g_ImmediateContext->PSSetConstantBuffers(3, 1, &g_MaterialBuffer);

	hBufferDesc.ByteWidth = sizeof(LIGHT_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_LightBuffer);
	g_ImmediateContext->VSSetConstantBuffers(4, 1, &g_LightBuffer);
	g_ImmediateContext->PSSetConstantBuffers(4, 1, &g_LightBuffer);

	hBufferDesc.ByteWidth = sizeof(FOG_CBUFFER);
	g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_FogBuffer);
	g_ImmediateContext->VSSetConstantBuffers(5, 1, &g_FogBuffer);
	g_ImmediateContext->PSSetConstantBuffers(5, 1, &g_FogBuffer);

	hBufferDesc.ByteWidth = sizeof(FUCHI);
	g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_LightIndexBuffer);
	g_ImmediateContext->VSSetConstantBuffers(6, 1, &g_LightIndexBuffer);
	g_ImmediateContext->PSSetConstantBuffers(6, 1, &g_LightIndexBuffer);

	hBufferDesc.ByteWidth = sizeof(XMFLOAT4);
	g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_CameraBuffer);
	g_ImmediateContext->VSSetConstantBuffers(7, 1, &g_CameraBuffer);
	g_ImmediateContext->PSSetConstantBuffers(7, 1, &g_CameraBuffer);

	hBufferDesc.ByteWidth = sizeof(LightViewsBuffer);
	hr = g_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &g_LightViewBuffer);
	g_ImmediateContext->VSSetConstantBuffers(8, 1, &g_LightViewBuffer);
	g_ImmediateContext->PSSetConstantBuffers(8, 1, &g_LightViewBuffer);

	g_ImmediateContext->IASetInputLayout(g_VertexLayout);

	g_ImmediateContext->VSSetShader(g_VertexShader, nullptr, 0);
	g_ImmediateContext->PSSetShader(g_PixelShader, nullptr, 0);

	ZeroMemory(&g_Light, sizeof(LIGHT_CBUFFER));
	g_Light.Direction[0] = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	g_Light.Diffuse[0] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_Light.Ambient[0] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	g_Light.Flags[0].Type = LIGHT_TYPE_DIRECTIONAL;
	SetLightBuffer();

	MATERIAL material{};
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	InitSkyBox();

	return S_OK;
}

void InitSkyBox()
{
	D3DX11_IMAGE_LOAD_INFO imgInfo{};
	imgInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = D3DX11CreateTextureFromFile(g_D3DDevice,
		g_SkyBoxPath.c_str(), &imgInfo, nullptr,
		reinterpret_cast<ID3D11Resource**>(SkyBox::g_SkyBoxTex.ReleaseAndGetAddressOf()), nullptr);

	D3D11_TEXTURE2D_DESC skyTexDesc{};
	SkyBox::g_SkyBoxTex->GetDesc(&skyTexDesc);
	const auto srvDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURECUBE, skyTexDesc.Format);

	g_D3DDevice->CreateShaderResourceView(SkyBox::g_SkyBoxTex.Get(), &srvDesc, SkyBox::g_ShaderResourceView.ReleaseAndGetAddressOf());

	ID3DBlob* pErrorBlob;
	ID3DBlob* pVsBlob = nullptr;
	DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG) && defined(DEBUG_SHADER)
	shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	hr = D3DX11CompileFromFile("shader.hlsl", nullptr, nullptr, "SkyBoxVS", "vs_5_0", shFlag, 0, nullptr,
		&pVsBlob, &pErrorBlob, nullptr);
	if (FAILED(hr))
		MessageBox(nullptr, static_cast<char*>(pErrorBlob->GetBufferPointer()), "VS", MB_OK | MB_ICONERROR);

	g_D3DDevice->CreateVertexShader(pVsBlob->GetBufferPointer(), pVsBlob->GetBufferSize(), nullptr, &SkyBox::g_VertexShader);
	pVsBlob->Release();

	ID3DBlob* pPsBlob = nullptr;
	hr = D3DX11CompileFromFile("shader.hlsl", nullptr, nullptr, "SkyBoxPS", "ps_5_0", shFlag, 0, nullptr, &pPsBlob, &pErrorBlob, nullptr);
	if (FAILED(hr))
	{
		MessageBox(nullptr, static_cast<char*>(pErrorBlob->GetBufferPointer()), "PS", MB_OK | MB_ICONERROR);
	}

	g_D3DDevice->CreatePixelShader(pPsBlob->GetBufferPointer(), pPsBlob->GetBufferSize(), nullptr, &SkyBox::g_PixelShader);

	pPsBlob->Release();

	CreateSphere(10, 10);
}

void UninitRenderer(void)
{
	if (g_DepthStateEnable)		g_DepthStateEnable->Release();
	if (g_DepthStateDisable)	g_DepthStateDisable->Release();
	if (g_BlendStateNone)		g_BlendStateNone->Release();
	if (g_BlendStateAlphaBlend)	g_BlendStateAlphaBlend->Release();
	if (g_BlendStateAdd)		g_BlendStateAdd->Release();
	if (g_BlendStateSubtract)	g_BlendStateSubtract->Release();
	if (g_RasterStateCullOff)	g_RasterStateCullOff->Release();
	if (g_RasterStateCullCW)	g_RasterStateCullCW->Release();
	if (g_RasterStateCullCCW)	g_RasterStateCullCCW->Release();

	if (g_WorldBuffer)			g_WorldBuffer->Release();
	if (g_ViewBuffer)			g_ViewBuffer->Release();
	if (g_ProjectionBuffer)		g_ProjectionBuffer->Release();
	if (g_MaterialBuffer)		g_MaterialBuffer->Release();
	if (g_LightBuffer)			g_LightBuffer->Release();
	if (g_FogBuffer)			g_FogBuffer->Release();
	if (g_LightViewBuffer)		g_LightViewBuffer->Release();

	if (g_VertexLayout)			g_VertexLayout->Release();
	if (g_VertexShader)			g_VertexShader->Release();
	if (g_PixelShader)			g_PixelShader->Release();

	if (g_ImmediateContext)		g_ImmediateContext->ClearState();
	if (g_RenderTargetView)		g_RenderTargetView->Release();
	if (g_SwapChain)			g_SwapChain->Release();
	if (g_ImmediateContext)		g_ImmediateContext->Release();
	if (g_D3DDevice)			g_D3DDevice->Release();

}

void Clear(void)
{
	const float ClearColor[4] = { 0,0,0,1 };     
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetView, ClearColor);
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Present(void)
{
	g_SwapChain->Present(0, 0);
}

void CreateSphere(int LatLines, int LongLines)
{
	SkyBox::g_NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	SkyBox::g_NumSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

	std::vector<VERTEX_3D> vertices(SkyBox::g_NumSphereVertices);

	vertices[0].Position.x = 0.0f;
	vertices[0].Position.y = 0.0f;
	vertices[0].Position.z = 1.0f;

	for (unsigned long i = 0; i < LatLines - 2; ++i)
	{
		float spherePitch = (i + 1) * (3.14 / (LatLines - 1));
		XMMATRIX Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < LongLines; ++j)
		{
			float sphereYaw = j * (6.28 / (LongLines));
			const XMMATRIX Rotationy = XMMatrixRotationZ(sphereYaw);
			XMVECTOR currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i * LongLines + j + 1].Position.x = XMVectorGetX(currVertPos);
			vertices[i * LongLines + j + 1].Position.y = XMVectorGetY(currVertPos);
			vertices[i * LongLines + j + 1].Position.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[SkyBox::g_NumSphereVertices - 1].Position.x = 0.0f;
	vertices[SkyBox::g_NumSphereVertices - 1].Position.y = 0.0f;
	vertices[SkyBox::g_NumSphereVertices - 1].Position.z = -1.0f;

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D) * SkyBox::g_NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	g_D3DDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &SkyBox::g_VertBuff);

	std::vector<DWORD> indices(SkyBox::g_NumSphereFaces * 3);

	int k = 0;
	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < LatLines - 3; ++i)
	{
		for (DWORD j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i * LongLines + j + 1;
			indices[k + 1] = i * LongLines + j + 2;
			indices[k + 2] = (i + 1) * LongLines + j + 1;

			indices[k + 3] = (i + 1) * LongLines + j + 1;
			indices[k + 4] = i * LongLines + j + 2;
			indices[k + 5] = (i + 1) * LongLines + j + 2;

			k += 6;   
		}

		indices[k] = (i * LongLines) + LongLines;
		indices[k + 1] = (i * LongLines) + 1;
		indices[k + 2] = ((i + 1) * LongLines) + LongLines;

		indices[k + 3] = ((i + 1) * LongLines) + LongLines;
		indices[k + 4] = (i * LongLines) + 1;
		indices[k + 5] = ((i + 1) * LongLines) + 1;

		k += 6;
	}

	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = SkyBox::g_NumSphereVertices - 1;
		indices[k + 1] = (SkyBox::g_NumSphereVertices - 1) - (l + 1);
		indices[k + 2] = (SkyBox::g_NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = SkyBox::g_NumSphereVertices - 1;
	indices[k + 1] = (SkyBox::g_NumSphereVertices - 1) - LongLines;
	indices[k + 2] = SkyBox::g_NumSphereVertices - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * SkyBox::g_NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	g_D3DDevice->CreateBuffer(&indexBufferDesc, &iinitData, &SkyBox::g_IdxBuff);
}

void DebugTextOut(char* text, int x, int y)
{
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)

	IDXGISurface1* pBackSurface = NULL;
	HRESULT hr = g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pBackSurface);

	if (SUCCEEDED(hr))
	{
		HDC hdc;
		try
		{
			hr = pBackSurface->GetDC(FALSE, &hdc);	        
		}
		catch (_com_error& error)
		{
			std::cout << error.ErrorMessage() << std::endl;
		}

		if (SUCCEEDED(hr))
		{
			SetTextColor(hdc, RGB(255, 255, 255));
			SetBkMode(hdc, TRANSPARENT);

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = SCREEN_WIDTH;
			rect.bottom = SCREEN_HEIGHT;

			DrawText(hdc, text, (int)strlen(text), &rect, DT_LEFT);

			pBackSurface->ReleaseDC(NULL);
		}
		pBackSurface->Release();

		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}
#endif
}