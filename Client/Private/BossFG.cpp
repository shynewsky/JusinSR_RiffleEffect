#include "stdafx.h"
#include "..\Public\BossFG.h"

#include "GameInstance.h"

CBossFG::CBossFG(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CBossFG::CBossFG(const CBossFG & Prototype)
	: CImage{ Prototype }
{
}

CBossFG * CBossFG::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossFG*		pInstance = new CBossFG(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBossFG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CBossFG::Clone(void* pArg)
{
	CBossFG*		pInstance = new CBossFG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBossFG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossFG::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
