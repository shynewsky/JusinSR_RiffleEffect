#include "..\Public\VIBuffer_Sphere.h"
#include <iostream>
CVIBuffer_Sphere::CVIBuffer_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere & Prototype)
	: CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_Sphere::Initialize_Prototype()
{
	const int numSlices = 30;
	const int numStacks = 30;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = (numSlices+1)* (numStacks + 1);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
	m_iNumFaces = numSlices * numStacks *2;
	m_iIndexStride = 4;
	m_iNumIndices = m_iNumFaces * 3;
	m_eIndexFormat = D3DFMT_INDEX32;


	/* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;	

	VTXNORTEX*			pVertices = { nullptr };
	
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	const float dTheta = -(D3DX_PI * 2) / float(numSlices);
	const float dPhi = -(D3DX_PI) / float(numStacks);
	const float radius = 1.0f;

	for (size_t i = 0; i <= numStacks; i++)
	{
		float phi = D3DX_PI / 2.0f + dPhi * i; 

		_float3 stackStartPoint;
		stackStartPoint.x = 0;
		stackStartPoint.y = -radius * sin(phi);
		stackStartPoint.z = radius * cos(phi);

		for (size_t j = 0; j <= numSlices; j++)
		{
			float theta = dTheta * j;
			_uint iIndex = (numSlices + 1) * i + j;
			pVertices[iIndex].vPosition.x = stackStartPoint.z * cos(theta) - stackStartPoint.x * sin(theta);
			pVertices[iIndex].vPosition.y = stackStartPoint.y;
			pVertices[iIndex].vPosition.z = -stackStartPoint.z * sin(theta) + stackStartPoint.x * cos(theta);

			pVertices[iIndex].vNormal = *D3DXVec3Normalize(&pVertices[iIndex].vNormal, &pVertices[iIndex].vPosition);
			
			pVertices[iIndex].vTexcoord.x = float(j) / numSlices;
			pVertices[iIndex].vTexcoord.y = 1.0f - float(i) / numStacks;
		}
	}

	m_pVB->Unlock();	

	if (FAILED(Ready_IndexBuffer()))
		return E_FAIL;

	_uint*		pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint		iNumIndices = { 0 };

	for (int i = 0; i < numStacks; i++) {
		const int offset = (numSlices + 1) * i;

		for (int j = 0; j < numSlices; j++) {
			_uint iIndices[6] = {
				offset + j,
				offset + j + numSlices + 1,
				offset + j + 1 + numSlices + 1,

				offset + j,
				offset + j + 1 + numSlices + 1,
				offset + j + 1
			};

			for (int k = 0; k < 6; k++) {
				pIndices[iNumIndices++] = iIndices[k];
			}
		}
	}
	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void * pArg)
{
	return S_OK;
}

CVIBuffer_Sphere * CVIBuffer_Sphere::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Sphere*		pInstance = new CVIBuffer_Sphere(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Sphere::Clone(void* pArg)
{
	CVIBuffer_Sphere*		pInstance = new CVIBuffer_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();

}

