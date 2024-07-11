#include "..\Public\VIBuffer_Cylinder.h"
#include <iostream>
#include <io.h>
CVIBuffer_Cylinder::CVIBuffer_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Cylinder::CVIBuffer_Cylinder(const CVIBuffer_Cylinder & Prototype)
	: CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_Cylinder::Initialize_Prototype(const float bottomRadius,
	const float topRadius, float height,
	int sliceCount)
{
	
	//const float XM_2PI = 6.283185307f;
	const float dTheta = -(D3DX_PI * 2) / float(sliceCount);

	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = (sliceCount + 1) * 2;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
	m_iNumFaces = (sliceCount  * 2);
	m_iIndexStride = 4;
	m_iNumIndices = m_iNumFaces * 3;
	m_eIndexFormat = D3DFMT_INDEX32;


	/* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
	if (FAILED(__super::Ready_VertexBuffer()))
	{
		return E_FAIL;	
	}


	VTXNORTEX*			pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	// Bottom vertices

	
	for (int i = 0; i <= sliceCount; i++) 
	{
		float theta = dTheta * float(i);
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		//// Position
		pVertices[i].vPosition.x = bottomRadius * cosTheta;
		pVertices[i].vPosition.y = -0.5f * height;
		pVertices[i].vPosition.z = -bottomRadius * sinTheta;

		pVertices[i].vNormal = pVertices[i].vPosition - _float3(0.0f, -0.5f * height, 0.0f);
		// TexCoord
		pVertices[i].vTexcoord.x = float(i) / sliceCount;
		pVertices[i].vTexcoord.y = 1.0f;
	}

	// Top vertices
	for (int i = 0; i <= sliceCount; i++) 
	{
		float theta = dTheta * float(i);
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		// Position
		pVertices[sliceCount + 1 + i].vPosition.x = topRadius * cosTheta;
		pVertices[sliceCount + 1 + i].vPosition.y = 0.5f * height;
		pVertices[sliceCount + 1 + i].vPosition.z = -topRadius * sinTheta;

		pVertices[sliceCount + 1 + i].vNormal = pVertices[sliceCount + 1 + i].vPosition - _float3(0.0f, 0.5f * height, 0.0f);

		// TexCoord
		pVertices[sliceCount + 1 + i].vTexcoord.x = float(i) / sliceCount;
		pVertices[sliceCount + 1 + i].vTexcoord.y = 0.0f;
	}

	m_pVB->Unlock();	
	if (FAILED(Ready_IndexBuffer()))
	{
		return E_FAIL;
	}

	_uint*		pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint		iNumIndices = { 0 };
	for (size_t i = 0; i < sliceCount ; i++)
	{
		_uint		iIndices[6] = {
			i , 
			i + sliceCount + 1,
			i + 1 + sliceCount + 1,
			i, 
			i + 1 + sliceCount + 1,
			i + 1
		};

		pIndices[iNumIndices++] = iIndices[0];
		pIndices[iNumIndices++] = iIndices[1];
		pIndices[iNumIndices++] = iIndices[2];

		pIndices[iNumIndices++] = iIndices[3];
		pIndices[iNumIndices++] = iIndices[4];
		pIndices[iNumIndices++] = iIndices[5];

	}

	m_pIB->Unlock();

	for (int i = 0; i < m_iNumIndices; ++i)
	{
	}

	return S_OK;
}

HRESULT CVIBuffer_Cylinder::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Cylinder::Render()
{
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumFaces);

	return S_OK;
}

CVIBuffer_Cylinder * CVIBuffer_Cylinder::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const float bottomRadius,
	const float topRadius, float height, int sliceCount)
{
	CVIBuffer_Cylinder*		pInstance = new CVIBuffer_Cylinder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(bottomRadius, topRadius, height, sliceCount)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Cylinder::Clone(void* pArg)
{
	CVIBuffer_Cylinder*		pInstance = new CVIBuffer_Cylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cylinder::Free()
{
	__super::Free();

}

