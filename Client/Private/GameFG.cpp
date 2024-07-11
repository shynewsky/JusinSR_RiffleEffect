#include "stdafx.h"
#include "..\Public\GameFG.h"

#include "GameInstance.h"

CGameFG::CGameFG(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CGameFG::CGameFG(const CGameFG & Prototype)
	: CImage{ Prototype }
{
}

CGameFG * CGameFG::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGameFG*		pInstance = new CGameFG(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGameFG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CGameFG::Clone(void* pArg)
{
	CGameFG*		pInstance = new CGameFG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGameFG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameFG::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
