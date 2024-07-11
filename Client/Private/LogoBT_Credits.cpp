#include "stdafx.h"
#include "..\Public\LogoBT_Credits.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLogoBT_Credits::CLogoBT_Credits(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CButton{ pGraphic_Device }
{
}

CLogoBT_Credits::CLogoBT_Credits(const CLogoBT_Credits & Prototype)
	: CButton{ Prototype }
{
}

CLogoBT_Credits * CLogoBT_Credits::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoBT_Credits*		pInstance = new CLogoBT_Credits(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Credits"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLogoBT_Credits::Clone(void* pArg)
{
	CLogoBT_Credits*		pInstance = new CLogoBT_Credits(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Credits"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBT_Credits::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
