#include "stdafx.h"
#include "..\Public\BossBG.h"

#include "GameInstance.h"

CBossBG::CBossBG(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CBossBG::CBossBG(const CBossBG & Prototype)
	: CImage{ Prototype }
{
}

CBossBG * CBossBG::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossBG*		pInstance = new CBossBG(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBossBG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CBossBG::Clone(void* pArg)
{
	CBossBG*		pInstance = new CBossBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBossBG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossBG::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
