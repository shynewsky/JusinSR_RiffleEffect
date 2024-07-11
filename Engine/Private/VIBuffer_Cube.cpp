#include "..\Public\VIBuffer_Cube.h"
#include "GameInstance.h"

CVIBuffer_Cube::CVIBuffer_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube & Prototype)
	: CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXNORCUBE);
	m_iNumVertices = 8;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	m_iNumFaces = 12;
	m_iIndexStride = 2;
	m_iNumIndices = 36;
	m_eIndexFormat = D3DFMT_INDEX16;

	/* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;	

	VTXNORCUBE*			pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_pVerticesPos[0] =  _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vNormal = *D3DXVec3Normalize(&pVertices[0].vNormal, &m_pVerticesPos[0]);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	pVertices[1].vPosition = m_pVerticesPos[1] = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vNormal = *D3DXVec3Normalize(&pVertices[1].vNormal, &m_pVerticesPos[1]);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = m_pVerticesPos[2] = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vNormal = *D3DXVec3Normalize(&pVertices[2].vNormal, &m_pVerticesPos[2]);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = m_pVerticesPos[3] = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vNormal = *D3DXVec3Normalize(&pVertices[3].vNormal, &m_pVerticesPos[3]);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = m_pVerticesPos[4] = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vNormal = *D3DXVec3Normalize(&pVertices[4].vNormal, &m_pVerticesPos[4]);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = m_pVerticesPos[5] = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vNormal = *D3DXVec3Normalize(&pVertices[5].vNormal, &m_pVerticesPos[5]);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = m_pVerticesPos[6] = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vNormal = *D3DXVec3Normalize(&pVertices[6].vNormal, &m_pVerticesPos[6]);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = m_pVerticesPos[7] = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vNormal = *D3DXVec3Normalize(&pVertices[7].vNormal, &m_pVerticesPos[7]);
	pVertices[7].vTexcoord = pVertices[7].vPosition;
	m_pVB->Unlock();	

	if (FAILED(Ready_IndexBuffer()))
		return E_FAIL;

	_ushort*		pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_pIB->Unlock();



	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void * pArg)
{
	return S_OK;
}

_bool CVIBuffer_Cube::isPicking(const _float4x4& WorldMatrix, _float3* pOut, _float* pDist)
{
	m_pGameInstance->Transform_MouseRay_ToLocalSpace(WorldMatrix);

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[1],
		m_pVerticesPos[5],
		m_pVerticesPos[6],
		pOut, pDist))
		goto Compute_WorldPos;


	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[1],
		m_pVerticesPos[6],
		m_pVerticesPos[2],
		pOut, pDist))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[4],
		m_pVerticesPos[0],
		m_pVerticesPos[3],
		pOut, pDist))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[4],
		m_pVerticesPos[3],
		m_pVerticesPos[7],
		pOut, pDist))
		goto Compute_WorldPos;

	/*if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[4],
		m_pVerticesPos[5],
		m_pVerticesPos[1],
		pOut))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[4],
		m_pVerticesPos[1],
		m_pVerticesPos[0],
		pOut))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[3],
		m_pVerticesPos[2],
		m_pVerticesPos[6],
		pOut))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[3],
		m_pVerticesPos[6],
		m_pVerticesPos[7],
		pOut))
		goto Compute_WorldPos;*/

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[5],
		m_pVerticesPos[4],
		m_pVerticesPos[7],
		pOut, pDist))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[5],
		m_pVerticesPos[7],
		m_pVerticesPos[6],
		pOut, pDist))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[0],
		m_pVerticesPos[1],
		m_pVerticesPos[2],
		pOut, pDist))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[0],
		m_pVerticesPos[2],
		m_pVerticesPos[3],
		pOut, pDist))
		goto Compute_WorldPos;

	return false;

Compute_WorldPos:
	D3DXVec3TransformCoord(pOut, pOut, &WorldMatrix);

	_float4x4		ViewMatrix{};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3 Length = *pOut - *(_float3*)&ViewMatrix.m[3];

	*pDist = D3DXVec3Length(&Length);


	return true;
}

_bool CVIBuffer_Cube::isPicked_InLocalSpaceForBlending(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
	_float		fU{}, fV{}, fDist{};

	if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos_InLocalSpace, &m_vRayDir_InLocalSpace, &fU, &fV, &fDist))
	{
		*pOut = m_vRayPos_InLocalSpace + m_vRayDir_InLocalSpace * fDist;
		return true;
	}

	return false;
}


_bool CVIBuffer_Cube::isBlinding(const _float4x4& WorldMatrix, _float3* pOut)
{

	/* 투영스페이스 == 로컬 * 월드행렬 * 뷰행렬 * 투영행렬 /w */
	_float3		vMousePos{};
	vMousePos.x = 0.f;//ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = -0.5f;//ptMouse.y / (m_iWinSizeY * -0.5f) + 1.f;
	vMousePos.z = 0.f; /* Near평면을 클릭한거야!! */

	/* 뷰스페이스 == 로컬 * 월드행렬 * 뷰행렬 */
	_float4x4			ProjMatrixInv{};
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

	/* 뷰스페이스 상에서의 마우스 레이와 레이의 시작점을 구해놓는다. */
	/* 뷰스페이스 상이기 때문에 카메라 좌표는 원점이다. */
	_float3 m_vRayPos = _float3(0.f, 0.f, 0.f);
	_float3 m_vRayDir = vMousePos - m_vRayPos;

	/* 월드스페이스 == 로컬 * 월드행렬 */
	_float4x4			ViewMatrixInv{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);

	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);

	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
	//========================================Update

	_float4x4		WorldMatrixInverse = *D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);


	D3DXVec3TransformCoord(&m_vRayPos_InLocalSpace, &m_vRayPos, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&m_vRayDir_InLocalSpace, &m_vRayDir, &WorldMatrixInverse);

	D3DXVec3Normalize(&m_vRayDir_InLocalSpace, &m_vRayDir_InLocalSpace);

	//======================================Transform_ToLocalSpace


	if (true == isPicked_InLocalSpaceForBlending(m_pVerticesPos[1],
		m_pVerticesPos[5],
		m_pVerticesPos[6],
		pOut))
		goto Compute_WorldPos;


	if (true == isPicked_InLocalSpaceForBlending(m_pVerticesPos[1],
		m_pVerticesPos[6],
		m_pVerticesPos[2],
		pOut))
		goto Compute_WorldPos;

	if (true == isPicked_InLocalSpaceForBlending(m_pVerticesPos[4],
		m_pVerticesPos[0],
		m_pVerticesPos[3],
		pOut))
		goto Compute_WorldPos;

	if (true == isPicked_InLocalSpaceForBlending(m_pVerticesPos[4],
		m_pVerticesPos[3],
		m_pVerticesPos[7],
		pOut))
		goto Compute_WorldPos;

	/*if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[4],
		m_pVerticesPos[5],
		m_pVerticesPos[1],
		pOut))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[4],
		m_pVerticesPos[1],
		m_pVerticesPos[0],
		pOut))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[3],
		m_pVerticesPos[2],
		m_pVerticesPos[6],
		pOut))
		goto Compute_WorldPos;

	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[3],
		m_pVerticesPos[6],
		m_pVerticesPos[7],
		pOut))
		goto Compute_WorldPos;*/

	if (true == isPicked_InLocalSpaceForBlending(m_pVerticesPos[5],
		m_pVerticesPos[4],
		m_pVerticesPos[7],
		pOut))
		goto Compute_WorldPos;

	if (true == isPicked_InLocalSpaceForBlending(m_pVerticesPos[5],
		m_pVerticesPos[7],
		m_pVerticesPos[6],
		pOut))
		goto Compute_WorldPos;

	if (true == isPicked_InLocalSpaceForBlending(m_pVerticesPos[0],
		m_pVerticesPos[1],
		m_pVerticesPos[2],
		pOut))
		goto Compute_WorldPos;

	if (true == isPicked_InLocalSpaceForBlending(m_pVerticesPos[0],
		m_pVerticesPos[2],
		m_pVerticesPos[3],
		pOut))
		goto Compute_WorldPos;

	return false;

Compute_WorldPos:
	D3DXVec3TransformCoord(pOut, pOut, &WorldMatrix);
	return true;
}

CVIBuffer_Cube * CVIBuffer_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();

}

