#include "stdafx.h"
#include "..\Public\GunSlot3_FlameShaker.h"

#include "GameInstance.h"

CGunSlot3_FlameShaker::CGunSlot3_FlameShaker(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CGunSlot3_FlameShaker::CGunSlot3_FlameShaker(const CGunSlot3_FlameShaker& Prototype)
	: CImage{ Prototype }
{
}

HRESULT CGunSlot3_FlameShaker::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_tImageInfo.fX - g_iWinSizeX * 0.5f, -m_tImageInfo.fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

HRESULT CGunSlot3_FlameShaker::Render()
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

CGunSlot3_FlameShaker* CGunSlot3_FlameShaker::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGunSlot3_FlameShaker* pInstance = new CGunSlot3_FlameShaker(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGunSlot3_FlameShaker"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CGunSlot3_FlameShaker::Clone(void* pArg)
{
	CGunSlot3_FlameShaker* pInstance = new CGunSlot3_FlameShaker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGunSlot3_FlameShaker"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunSlot3_FlameShaker::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
