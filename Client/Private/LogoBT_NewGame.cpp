#include "stdafx.h"
#include "..\Public\LogoBT_NewGame.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLogoBT_NewGame::CLogoBT_NewGame(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CButton{ pGraphic_Device }
{
}

CLogoBT_NewGame::CLogoBT_NewGame(const CLogoBT_NewGame & Prototype)
	: CButton{ Prototype }
{
}

CLogoBT_NewGame * CLogoBT_NewGame::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoBT_NewGame*		pInstance = new CLogoBT_NewGame(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_NewGame"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLogoBT_NewGame::Clone(void* pArg)
{
	CLogoBT_NewGame*		pInstance = new CLogoBT_NewGame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_NewGame"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBT_NewGame::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
