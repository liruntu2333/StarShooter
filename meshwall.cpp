#include "main.h"
#include "input.h"
#include "meshwall.h"
#include "renderer.h"

#define TEXTURE_MAX			(1)						 

#define	MAX_MESH_WALL		(10)					 
#define	VALUE_MOVE_WALL		(5.0f)					 
#define	VALUE_ROTATE_WALL	(D3DX_PI * 0.001f)		 

typedef struct
{
	ID3D11Buffer* vertexBuffer;	 
	ID3D11Buffer* indexBuffer;	 

	XMFLOAT3		pos;						 
	XMFLOAT3		rot;						 
	MATERIAL		material;					 
	int				nNumBlockX, nNumBlockY;		 
	int				nNumVertex;					 
	int				nNumVertexIndex;			 
	int				nNumPolygon;				 
	float			fBlockSizeX, fBlockSizeY;	 
} MESH_WALL;

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 
static int							g_TexNo;		 

static MESH_WALL g_aMeshWall[MAX_MESH_WALL];		 
static int g_nNumMeshWall = 0;						 

static char* g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/wallpaper.png",
};

static BOOL							g_Load = FALSE;

HRESULT InitMeshWall(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 col,
	int nNumBlockX, int nNumBlockY, float fBlockSizeX, float fBlockSizeZ)
{
	MESH_WALL* pMesh;

	if (g_nNumMeshWall >= MAX_MESH_WALL)
	{
		return E_FAIL;
	}

	if (g_nNumMeshWall == 0)
	{
		for (int i = 0; i < TEXTURE_MAX; i++)
		{
			D3DX11CreateShaderResourceViewFromFile(GetDevice(),
				g_TextureName[i],
				nullptr,
				nullptr,
				&g_Texture[i],
			nullptr);
		}
	}

	g_TexNo = 0;

	pMesh = &g_aMeshWall[g_nNumMeshWall];

	g_nNumMeshWall++;

	ZeroMemory(&pMesh->material, sizeof(pMesh->material));
	pMesh->material.Diffuse = col;
	pMesh->material.Specular = { 0.5f, 0.5f, 0.5f, 1.0f };

	pMesh->pos = pos;

	pMesh->rot = rot;

	pMesh->nNumBlockX = nNumBlockX;
	pMesh->nNumBlockY = nNumBlockY;

	pMesh->nNumVertex = (nNumBlockX + 1) * (nNumBlockY + 1);

	pMesh->nNumVertexIndex = (nNumBlockX + 1) * 2 * nNumBlockY + (nNumBlockY - 1) * 2;

	pMesh->nNumPolygon = nNumBlockX * nNumBlockY * 2 + (nNumBlockY - 1) * 4;

	pMesh->fBlockSizeX = fBlockSizeX;
	pMesh->fBlockSizeY = fBlockSizeZ;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * pMesh->nNumVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, nullptr, &pMesh->vertexBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * pMesh->nNumVertexIndex;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, nullptr, &pMesh->indexBuffer);

	{
#if 0
		const float texSizeX = 1.0f / g_nNumBlockX;
		const float texSizeZ = 1.0f / g_nNumBlockY;
#else
		const float texSizeX = 1.0f;
		const float texSizeZ = 1.0f;
#endif

		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(pMesh->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		const auto pVtx = static_cast<VERTEX_3D*>(msr.pData);

		for (int nCntVtxY = 0; nCntVtxY < (pMesh->nNumBlockY + 1); nCntVtxY++)
		{
			for (int nCntVtxX = 0; nCntVtxX < (pMesh->nNumBlockX + 1); nCntVtxX++)
			{
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Position.x = -(pMesh->nNumBlockX / 2.0f) * pMesh->fBlockSizeX + nCntVtxX * pMesh->fBlockSizeX;
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Position.y = pMesh->nNumBlockY * pMesh->fBlockSizeY - nCntVtxY * pMesh->fBlockSizeY;
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Position.z = 0.0f;

				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].TexCoord.x = texSizeX * nCntVtxX;
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].TexCoord.y = texSizeZ * nCntVtxY;
			}
		}

		GetDeviceContext()->Unmap(pMesh->vertexBuffer, 0);
	}

	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(pMesh->indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		const auto pIdx = static_cast<unsigned short*>(msr.pData);

		int nCntIdx = 0;
		for (int nCntVtxY = 0; nCntVtxY < pMesh->nNumBlockY; nCntVtxY++)
		{
			if (nCntVtxY > 0)
			{ 
				pIdx[nCntIdx] = (nCntVtxY + 1) * (pMesh->nNumBlockX + 1);
				nCntIdx++;
			}

			for (int nCntVtxX = 0; nCntVtxX < (pMesh->nNumBlockX + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxY + 1) * (pMesh->nNumBlockX + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX;
				nCntIdx++;
			}

			if (nCntVtxY < (pMesh->nNumBlockY - 1))
			{ 
				pIdx[nCntIdx] = nCntVtxY * (pMesh->nNumBlockX + 1) + pMesh->nNumBlockX;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(pMesh->indexBuffer, 0);
	}

	g_Load = TRUE;
	return S_OK;
}

void UninitMeshWall(void)
{
	MESH_WALL* pMesh;
	int nCntMeshField;

	if (g_Load == FALSE) return;

	for (nCntMeshField = 0; nCntMeshField < g_nNumMeshWall; nCntMeshField++)
	{
		pMesh = &g_aMeshWall[nCntMeshField];

		if (pMesh->vertexBuffer)
		{ 
			pMesh->vertexBuffer->Release();
			pMesh->vertexBuffer = nullptr;
		}

		if (pMesh->indexBuffer)
		{ 
			pMesh->indexBuffer->Release();
			pMesh->indexBuffer = nullptr;
		}
	}

	for (auto& i : g_Texture)
	{
		if (i)
		{
			i->Release();
			i = nullptr;
		}
	}

	g_nNumMeshWall = 0;

	g_Load = FALSE;
}

void UpdateMeshWall(void)
{
}

void DrawMeshWall(void)
{
	MESH_WALL* pMesh;
	int nCntMeshField;

	for (nCntMeshField = 0; nCntMeshField < g_nNumMeshWall; nCntMeshField++)
	{
		pMesh = &g_aMeshWall[nCntMeshField];

		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &pMesh->vertexBuffer, &stride, &offset);

		GetDeviceContext()->IASetIndexBuffer(pMesh->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		SetMaterial(pMesh->material);

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		XMMATRIX mtxRot, mtxTranslate, mtxWorld;

		mtxWorld = XMMatrixIdentity();

		mtxRot = XMMatrixRotationRollPitchYaw(pMesh->rot.x, pMesh->rot.y, pMesh->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		mtxTranslate = XMMatrixTranslation(pMesh->pos.x, pMesh->pos.y, pMesh->pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		SetWorldMatrix(&mtxWorld);

		GetDeviceContext()->DrawIndexed(pMesh->nNumVertexIndex, 0, 0);
	}

}