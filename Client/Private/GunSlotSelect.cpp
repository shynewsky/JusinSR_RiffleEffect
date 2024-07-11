#include "stdafx.h"
#include "..\Public\GunSlotSelect.h"

#include "GameInstance.h"

CGunSlotSelect::CGunSlotSelect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CGunSlotSelect::CGunSlotSelect(const CGunSlotSelect & Prototype)
	: CImage{ Prototype }
{
}

void CGunSlotSelect::Priority_Update(_float fTimeDelta)
{
}

int CGunSlotSelect::Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CGunSlotSelect::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	Set_SlotPos();
}

void CGunSlotSelect::Set_SlotPos()
{
	switch (m_eSlotNum)
	{
	case Client::CGunSlotSelect::SLOT1:
		m_tImageInfo.fY = 425.f;
		break;
	case Client::CGunSlotSelect::SLOT2:
		m_tImageInfo.fY = 485.f;
		break;
	case Client::CGunSlotSelect::SLOT3:
		m_tImageInfo.fY = 545.f;
		break;
	default:
		break;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_tImageInfo.fX - g_iWinSizeX * 0.5f, -m_tImageInfo.fY + g_iWinSizeY * 0.5f, 0.f));
}

CGunSlotSelect * CGunSlotSelect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGunSlotSelect*		pInstance = new CGunSlotSelect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGunSlotSelect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CGunSlotSelect::Clone(void* pArg)
{
	CGunSlotSelect*		pInstance = new CGunSlotSelect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGunSlotSelect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunSlotSelect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
