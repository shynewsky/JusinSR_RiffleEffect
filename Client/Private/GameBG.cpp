#include "stdafx.h"
#include "..\Public\GameBG.h"

#include "GameInstance.h"

CGameBG::CGameBG(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CGameBG::CGameBG(const CGameBG & Prototype)
	: CImage{ Prototype }
{
}

CGameBG * CGameBG::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGameBG*		pInstance = new CGameBG(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGameBG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CGameBG::Clone(void* pArg)
{
	CGameBG*		pInstance = new CGameBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGameBG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameBG::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
