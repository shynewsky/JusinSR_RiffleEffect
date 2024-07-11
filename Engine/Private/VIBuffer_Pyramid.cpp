#include "..\Public\VIBuffer_Pyramid.h"
#include <iostream>
CVIBuffer_Pyramid::CVIBuffer_Pyramid(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Pyramid::CVIBuffer_Pyramid(const CVIBuffer_Pyramid & Prototype)
	: CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_Pyramid::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXTEX);
	m_iNumVertices = 5;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
	m_iNumFaces = 6;
	m_iIndexStride = 2;
	m_iNumIndices = 18;
	m_eIndexFormat = D3DFMT_INDEX16;

	/* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;

	VTXTEX* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_pVerticesPos[0] = _float3(0.f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = m_pVerticesPos[1] = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = m_pVerticesPos[2] = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = _float2(1.f, 0.25f);

	pVertices[3].vPosition = m_pVerticesPos[3] = _float3(0.5f, -0.5f, 0.5f);
	pVertices[3].vTexcoord = _float2(1.f, 0.5f);

	pVertices[4].vPosition = m_pVerticesPos[4] = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[4].vTexcoord = _float2(1.f, 0.75f);


	m_pVB->Unlock();

	if (FAILED(Ready_IndexBuffer()))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	/* +x */
	pIndices[0] = 0; pIndices[1] = 2; pIndices[2] = 1;
	pIndices[3] = 0; pIndices[4] = 3; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 0; pIndices[7] = 4; pIndices[8] = 3;
	pIndices[9] = 0; pIndices[10] = 1; pIndices[11] = 4;

	/* +y */
	pIndices[12] = 1; pIndices[13] = 2; pIndices[14] = 4;
	pIndices[15] = 2; pIndices[16] = 3; pIndices[17] = 4;

	m_pIB->Unlock();



	return S_OK;
}

HRESULT CVIBuffer_Pyramid::Initialize(void * pArg)
{
	return S_OK;
}

CVIBuffer_Pyramid * CVIBuffer_Pyramid::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Pyramid*		pInstance = new CVIBuffer_Pyramid(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Pyramid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Pyramid::Clone(void* pArg)
{
	CVIBuffer_Pyramid*		pInstance = new CVIBuffer_Pyramid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Pyramid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Pyramid::Free()
{
	__super::Free();

}

