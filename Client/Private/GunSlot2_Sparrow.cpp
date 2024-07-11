#include "stdafx.h"
#include "..\Public\GunSlot2_Sparrow.h"

#include "GameInstance.h"

CGunSlot2_Sparrow::CGunSlot2_Sparrow(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CGunSlot2_Sparrow::CGunSlot2_Sparrow(const CGunSlot2_Sparrow& Prototype)
	: CImage{ Prototype }
{
}

HRESULT CGunSlot2_Sparrow::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_tImageInfo.fX - g_iWinSizeX * 0.5f, -m_tImageInfo.fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

HRESULT CGunSlot2_Sparrow::Render()
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

CGunSlot2_Sparrow* CGunSlot2_Sparrow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGunSlot2_Sparrow* pInstance = new CGunSlot2_Sparrow(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGunSlot2_Sparrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CGunSlot2_Sparrow::Clone(void* pArg)
{
	CGunSlot2_Sparrow* pInstance = new CGunSlot2_Sparrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGunSlot2_Sparrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunSlot2_Sparrow::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
