#include "stdafx.h"
#include "..\Public\LogoBT_Quit.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLogoBT_Quit::CLogoBT_Quit(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CButton{ pGraphic_Device }
{
}

CLogoBT_Quit::CLogoBT_Quit(const CLogoBT_Quit & Prototype)
	: CButton{ Prototype }
{
}

CLogoBT_Quit * CLogoBT_Quit::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoBT_Quit*		pInstance = new CLogoBT_Quit(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Quit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLogoBT_Quit::Clone(void* pArg)
{
	CLogoBT_Quit*		pInstance = new CLogoBT_Quit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Quit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBT_Quit::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
