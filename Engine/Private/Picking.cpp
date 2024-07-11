#include "..\Public\Picking.h"

CPicking::CPicking(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device { pGraphic_Device }
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;

	m_iWinSizeX = iWinSizeX;

	m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void CPicking::Update()
{
	POINT			ptMouse{};

	GetCursorPos(&ptMouse);

	/* 뷰포트 == 로컬 * 월드행렬 * 뷰행렬 * 투영행렬 /w -> 뷰포트 변환 */
	ScreenToClient(m_hWnd, &ptMouse);

	/* 투영스페이스 == 로컬 * 월드행렬 * 뷰행렬 * 투영행렬 /w */
	_float3		vMousePos{};
	vMousePos.x = 0.f;//ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = 0.f;//ptMouse.y / (m_iWinSizeY * -0.5f) + 1.f;
	vMousePos.z = 0.f; /* Near평면을 클릭한거야!! */

	/* 뷰스페이스 == 로컬 * 월드행렬 * 뷰행렬 */
	_float4x4			ProjMatrixInv{};
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

	/* 뷰스페이스 상에서의 마우스 레이와 레이의 시작점을 구해놓는다. */
	/* 뷰스페이스 상이기 때문에 카메라 좌표는 원점이다. */
	m_vRayPos = _float3(0.f, 0.f, 0.f);
	m_vRayDir = vMousePos - m_vRayPos;

	/* 월드스페이스 == 로컬 * 월드행렬 */
	_float4x4			ViewMatrixInv{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);

	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);

	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
}

void CPicking::Transform_ToLocalSpace(const _float4x4& WorldMatrix)
{
	_float4x4		WorldMatrixInverse = *D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);

	D3DXVec3TransformCoord(&m_vRayPos_InLocalSpace, &m_vRayPos, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&m_vRayDir_InLocalSpace, &m_vRayDir, &WorldMatrixInverse);		

	D3DXVec3Normalize(&m_vRayDir_InLocalSpace, &m_vRayDir_InLocalSpace);
}

_bool CPicking::isPicked_InWorldSpace(const _float3 & vPointA, const _float3 & vPointB, const _float3 & vPointC, _float3 * pOut)
{
	_float		fU{}, fV{}, fDist{};

	if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos, &m_vRayDir, &fU, &fV, &fDist))
	{
		*pOut = m_vRayPos + m_vRayDir * fDist;
		return true;
	}

	return false;
}

_bool CPicking::isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut, _float* pDist)
{
	_float		fU{}, fV{}, fDist{};
	_float3 edge1 = vPointB - vPointA;
	_float3 edge2 = vPointC - vPointA;

	_float3 normal;

	D3DXVec3Cross(&normal, &edge1, &edge2);
	D3DXVec3Normalize(&normal, &normal);

	_float dotProduct = D3DXVec3Dot(&normal, &m_vRayDir_InLocalSpace);

	if (dotProduct < 0.0f)
	{
		if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos_InLocalSpace, &m_vRayDir_InLocalSpace, &fU, &fV, &fDist))
		{
			fDist *= 0.98f;
			*pOut = m_vRayPos_InLocalSpace + m_vRayDir_InLocalSpace * fDist;
			*pDist = fDist;
			return true;
		}
	}

	return false;
}

_bool CPicking::isPicked_InLocalSpaceForBlending(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
	_float		fU{}, fV{}, fDist{};

	if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos_InLocalSpace, &m_vRayDir_InLocalSpace, &fU, &fV, &fDist))
	{
		*pOut = m_vRayPos_InLocalSpace + m_vRayDir_InLocalSpace * fDist;
		return true;
	}


	return false;
}

CPicking * CPicking::Create(LPDIRECT3DDEVICE9 pGraphic_Device, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CPicking*		pInstance = new CPicking(pGraphic_Device);

	if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed to Created : CPicking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	__super::Free();

	Safe_Release(m_pGraphic_Device);
}
