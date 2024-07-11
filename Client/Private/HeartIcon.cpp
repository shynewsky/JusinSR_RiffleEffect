#include "stdafx.h"
#include "..\Public\HeartIcon.h"

#include "GameInstance.h"

CHeartIcon::CHeartIcon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CHeartIcon::CHeartIcon(const CHeartIcon & Prototype)
	: CImage{ Prototype }
{
}

HRESULT CHeartIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHeartIcon::Initialize(void* pArg)
{
	Set_Variables(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_OrthoLH();

	return S_OK;
}

void CHeartIcon::Priority_Update(_float fTimeDelta)
{
}

int CHeartIcon::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	if (m_eHeartState == HEART_PUMP)
	{
		m_fFrame += fTimeDelta * m_tImageInfo.iFrameSpeed;
		if (m_fFrame > 7) 	// 0~7 까지는 평범, 8~19는 피격
			m_fFrame = 0.f;
	}
	else
	{
		m_fFrame += fTimeDelta * 14.f;
		if (m_fFrame + 8 >= 20) 	// 0~7 까지는 평범, 8~19는 피격
			m_bDead = true;
	}
	

	return OBJ_NOEVENT;
}

void CHeartIcon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CHeartIcon::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (m_eHeartState == HEART_PUMP) /* 타격을 입지 않은 경우 -> 0~7까지 출력 */
	{
		if (FAILED(m_pTextureCom->Bind_Texture(_uint(m_fFrame))))
			return E_FAIL;
	}
	else  if (m_eHeartState == HEART_POP) /*타격을 입은 경우 -> 8~19만 출력*/
	{
		if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame + 8)))
			return E_FAIL;

		if (m_dwTime + 1000 < GetTickCount())
		{
			m_bDead = true;
		}
	}

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

CHeartIcon * CHeartIcon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHeartIcon*		pInstance = new CHeartIcon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHeartIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CHeartIcon::Clone(void* pArg)
{
	CHeartIcon*		pInstance = new CHeartIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CHeartIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHeartIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
