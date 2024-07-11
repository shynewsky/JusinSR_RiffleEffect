#include "stdafx.h"
#include "..\Public\BossFace.h"

#include "GameInstance.h"

CBossFace::CBossFace(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CBossFace::CBossFace(const CBossFace & Prototype)
	: CImage{ Prototype }
{
}

CBossFace * CBossFace::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossFace*		pInstance = new CBossFace(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBossFace"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CBossFace::Clone(void* pArg)
{
	CBossFace*		pInstance = new CBossFace(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBossFace"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossFace::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
