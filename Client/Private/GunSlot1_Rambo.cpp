#include "stdafx.h"
#include "..\Public\GunSlot1_Rambo.h"

#include "GameInstance.h"

CGunSlot1_Rambo::CGunSlot1_Rambo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CGunSlot1_Rambo::CGunSlot1_Rambo(const CGunSlot1_Rambo& Prototype)
	: CImage{ Prototype }
{
}

HRESULT CGunSlot1_Rambo::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_tImageInfo.fX - g_iWinSizeX * 0.5f, -m_tImageInfo.fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

HRESULT CGunSlot1_Rambo::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(m_eGun)))
		return E_FAIL;

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

CGunSlot1_Rambo* CGunSlot1_Rambo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGunSlot1_Rambo* pInstance = new CGunSlot1_Rambo(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGunSlot1_Rambo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CGunSlot1_Rambo::Clone(void* pArg)
{
	CGunSlot1_Rambo* pInstance = new CGunSlot1_Rambo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGunSlot1_Rambo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunSlot1_Rambo::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
