#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "renderer.h"
#include "collision.h"
#include "MathHelper.h"

#define TEXTURE_MAX									(1)				 
#define ROAD_HALF_WIDTH								(50.0f)
#define ROAD_WIDTH									(ROAD_HALF_WIDTH * 2.0f)
#define WIDTH_BIAS									(10.0f)
#define DECISION_AREA_HALF_WIDTH					(20.0f)

static ID3D11Buffer* g_VertexBuffer = nullptr;	 
static ID3D11Buffer* g_IndexBuffer = nullptr;	 

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 
static int							g_TexNo;				 

static XMFLOAT3		g_posField;								 
static XMFLOAT3		g_rotField;								 

static int			g_nNumBlockXField, g_nNumBlockZField;	 
static int			g_nNumVertexField;						 
static int			g_nNumVertexIndexField;					 
static int			g_nNumPolygonField;						 
static float		g_fBlockSizeXField, g_fBlockSizeZField;	 
static float		g_FieldSizeX, g_FieldSizeZ;	 
static float		g_FieldHalfWidth, g_FieldHalfDepth;

static char* g_TextureName[] = {
	"data/TEXTURE/ground.jpg",
};

static VERTEX_3D* g_Vertex = nullptr;

static XMFLOAT3		g_Center;					 
static float		g_Time = 0.0f;				 
static float		g_wave_frequency = 2.0f;	 
static float		g_wave_correction = 0.02f;	 
static float		g_wave_amplitude = 20.0f;	 

static BOOL			g_Load = FALSE;

inline bool IsFacingWest(const float dir)
{
	const float sin = sinf(dir);
	return sin > 1.0f - 0.05f;
}
inline bool IsFacingEast(const float dir)
{
	const float sin = sinf(dir);
	return sin < -1.0f + 0.05f;
}
inline bool IsFacingNorth(const float dir)
{
	const float cos = cosf(dir);
	return cos < -1.0f + 0.05f;
}
inline bool IsFacingSouth(const float dir)
{
	const float cos = cosf(dir);
	return cos > 1.0f - 0.05f;
}

HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	g_posField = pos;

	g_rotField = rot;

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			nullptr,
			nullptr,
			&g_Texture[i],
		nullptr);
	}

	g_TexNo = 0;

	g_nNumBlockXField = nNumBlockX;
	g_nNumBlockZField = nNumBlockZ;

	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	g_fBlockSizeXField = fBlockSizeX;
	g_fBlockSizeZField = fBlockSizeZ;
	g_FieldSizeX = g_fBlockSizeXField * g_nNumBlockXField;
	g_FieldSizeZ = g_fBlockSizeZField * g_nNumBlockZField;
	g_FieldHalfWidth = g_FieldSizeX * 0.5f;
	g_FieldHalfDepth = g_FieldSizeZ * 0.5f;

	g_Vertex = new VERTEX_3D[g_nNumVertexField];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		 
	g_Time = 0.0f;								 
	g_wave_frequency = 1.0f;					 
	g_wave_correction = 0.02f;					 
	g_wave_amplitude = 30.0f;					 

	for (int z = 0; z < (g_nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (g_nNumBlockXField + 1); x++)
		{
			float& currX = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x;
			float& currZ = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z;
			float& currY = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y;

			currX = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
			currZ = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;
			currY = GetFieldHeight(currX, currZ);

			g_Vertex[z * (g_nNumBlockXField + 1) + x].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			g_Vertex[z * (g_nNumBlockXField + 1) + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.x = texSizeX * x;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.y = texSizeZ * z;
		}
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, nullptr, &g_IndexBuffer);

	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		const auto pVtx = static_cast<VERTEX_3D*>(msr.pData);

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexField);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		const auto pIdx = static_cast<unsigned short*>(msr.pData);

		int nCntIdx = 0;
		for (int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
		{
			if (nCntVtxZ > 0)
			{ 
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1);
				nCntIdx++;
			}

			for (int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if (nCntVtxZ < (g_nNumBlockZField - 1))
			{ 
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + g_nNumBlockXField;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(g_IndexBuffer, 0);
	}

	g_Load = TRUE;
	return S_OK;
}

void UninitMeshField(void)
{
	if (g_Load == FALSE) return;

	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = nullptr;
	}

	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = nullptr;
	}

	for (auto& i : g_Texture)
	{
		if (i)
		{
			i->Release();
			i = nullptr;
		}
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = nullptr;
	}

	g_Load = FALSE;
}

void UpdateMeshField(void)
{
	return;	 

	float dt = 0.03f;

	for (int z = 0; z < g_nNumBlockZField; z++)
	{
		for (int x = 0; x < g_nNumBlockXField; x++)
		{
			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			float len = static_cast<float>(sqrt(dx * dx + dz * dz));

			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
		}
	}
	g_Time += dt;

	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	auto pVtx = static_cast<VERTEX_3D*>(msr.pData);

	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}

void DrawMeshField(void)
{
	const UINT stride = sizeof(VERTEX_3D);
	const UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL material{};
	material.Diffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
	SetMaterial(material);

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	XMMATRIX mtxWorld = XMMatrixTranslation(0, 0, 0);

	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	XMMATRIX mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	SetWorldMatrix(&mtxWorld);

	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);
}

void DrawMeshFieldToDepthTex()
{
	constexpr UINT stride = sizeof(VERTEX_3D);
	constexpr UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	XMMATRIX mtxWorld = XMMatrixIdentity();

	const XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	const XMMATRIX mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	SetWorldMatrix(&mtxWorld);

	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);
}

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3* HitPosition, XMFLOAT3* Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;
	const XMFLOAT3 org = *HitPosition;

	start.y += 1000.0f;
	end.y -= 1000.0f;

	const float fz = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
	const float fx = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;
	int sz = static_cast<int>((-start.z + fz) / g_fBlockSizeZField);
	int sx = static_cast<int>((start.x + fx) / g_fBlockSizeXField);

	if ((sz < 0) || (sz > g_nNumBlockZField - 1) ||
		(sx < 0) || (sx > g_nNumBlockXField - 1))
	{
		*Normal = { 0.0f, 1.0f, 0.0f };
		return FALSE;
	}

	sz = 0;
	sx = 0;
	int ez = g_nNumBlockZField;
	int ex = g_nNumBlockXField;

	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			const XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position;
			const XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXField + 1) + (x + 1)].Position;
			const XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + x].Position;
			const XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + (x + 1)].Position;

			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}

	*HitPosition = org;
	return FALSE;
}

float GetFieldHeight(float x, float z)
{
	if ((x > -ROAD_HALF_WIDTH && x < +ROAD_HALF_WIDTH) ||
		(z > -ROAD_HALF_WIDTH && z < +ROAD_HALF_WIDTH))
	{
		return -5.0f;
	}
	return -30.f;
}

bool IsPositionValid(float x, float z)
{
	return GetFieldHeight(x - WIDTH_BIAS, z) > -10.0f && GetFieldHeight(x + WIDTH_BIAS, z) > -10.0f
		&& GetFieldHeight(x, z - WIDTH_BIAS) > -10.0f && GetFieldHeight(x, z + WIDTH_BIAS) > -10.0f;
}

XMFLOAT3 __vectorcall GetWrapPosition(XMFLOAT3 pos, int endOfBoarderFlag)
{
	if (endOfBoarderFlag & EndOfZPlus)       pos.z -= g_FieldSizeZ;
	else if (endOfBoarderFlag & EndOfZMinus) pos.z += g_FieldSizeZ;
	if (endOfBoarderFlag & EndOfXPlus)       pos.x -= g_FieldSizeX;
	else if (endOfBoarderFlag & EndOfXMinus) pos.x += g_FieldSizeX;
	return pos;
}

int IsOutOfBoarder(float x, float z)
{
	int flag = EndOfNone;
	if (z > +g_FieldSizeZ / 2.0f)      flag |= EndOfZPlus;
	else if (z < -g_FieldSizeZ / 2.0f) flag |= EndOfZMinus;
	if (x > +g_FieldSizeX / 2.0f)      flag |= EndOfXPlus;
	else if (x < -g_FieldSizeX / 2.0f) flag |= EndOfXMinus;
	return flag;
}

float GetFieldProgress(float x, float z, float dir)
{
	if (IsFacingEast(dir))
	{
		return 1.0f - (x + g_FieldHalfWidth) / g_FieldSizeX;
	}
	if (IsFacingWest(dir))
	{
		return 1.0f - (x + g_FieldHalfWidth) / g_FieldSizeX;
	}
	if (IsFacingNorth(dir))
	{
		return 1.0f - (z + g_FieldHalfDepth) / g_FieldSizeZ;
	}
	return 1.0f - (z + g_FieldHalfDepth) / g_FieldSizeZ;
}

int IsAtConjunction(const float x, const float z, const float dir)
{
	if (IsFacingEast(dir) || IsFacingWest(dir))
	{
		return x > -DECISION_AREA_HALF_WIDTH && x < +DECISION_AREA_HALF_WIDTH;
	}
	return z > -DECISION_AREA_HALF_WIDTH && z < +DECISION_AREA_HALF_WIDTH;
}

XMFLOAT3 GetRandomValidPosition()
{
	float x = 0.0, z = 0.0;

	if (rand() % 2)
	{
		x = MathHelper::RandF() * ROAD_WIDTH - ROAD_HALF_WIDTH;
		z = MathHelper::RandF() * g_FieldSizeZ - g_FieldHalfDepth;
	}
	else
	{
		x = MathHelper::RandF() * g_FieldSizeX - g_FieldHalfWidth;
		z = MathHelper::RandF() * ROAD_WIDTH - ROAD_HALF_WIDTH;
	}
	float y = GetFieldHeight(x, z);
	return { x, y, z };
}

XMFLOAT3 GetRandomPosition()
{
	const float x = (MathHelper::RandF() * g_FieldSizeX - g_FieldHalfWidth) * 0.9f;
	const float z = (MathHelper::RandF() * g_FieldSizeZ - g_FieldHalfDepth) * 0.9f;
	const float y = MathHelper::RandF() * 20.0f + 40.0f;

	return { x, y, z };
}