#include "stdafx.h"
#include "..\Public\GattoIcon.h"

#include "GameInstance.h"

CGattoIcon::CGattoIcon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CGattoIcon::CGattoIcon(const CGattoIcon & Prototype)
	: CImage{ Prototype }
{
}

HRESULT CGattoIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGattoIcon::Initialize(void* pArg)
{
	Set_Variables(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_OrthoLH();

	return S_OK;
}

void CGattoIcon::Priority_Update(_float fTimeDelta)
{
}

int CGattoIcon::Update(_float fTimeDelta)
{
	m_fFrame += fTimeDelta * m_tImageInfo.iFrameSpeed;

	if (m_fFrame > 7) 	// 0~7 까지는 평범, 8는 피격
		m_fFrame = 0.f;

	return OBJ_NOEVENT;
}

void CGattoIcon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CGattoIcon::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
	
	if (m_eGattoState == GATTO_NORMAL) /* 타격을 입지 않은 경우 -> 0~7까지 출력 */
	{
		if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame)))
			return E_FAIL;
	}
	else if(m_eGattoState == GATTO_DAMAGED) /*타격을 입은 경우 -> 8만 출력*/
	{
		if (FAILED(m_pTextureCom->Bind_Texture(8)))
			return E_FAIL;

		if (m_dwTime + 1000 < GetTickCount())
		{
			m_eGattoState = GATTO_NORMAL;
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

CGattoIcon * CGattoIcon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGattoIcon*		pInstance = new CGattoIcon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGattoIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CGattoIcon::Clone(void* pArg)
{
	CGattoIcon*		pInstance = new CGattoIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGattoIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGattoIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
