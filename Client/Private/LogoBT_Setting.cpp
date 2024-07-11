#include "stdafx.h"
#include "..\Public\LogoBT_Setting.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLogoBT_Setting::CLogoBT_Setting(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CButton{ pGraphic_Device }
{
}

CLogoBT_Setting::CLogoBT_Setting(const CLogoBT_Setting & Prototype)
	: CButton{ Prototype }
{
}

CLogoBT_Setting * CLogoBT_Setting::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoBT_Setting*		pInstance = new CLogoBT_Setting(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Setting"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLogoBT_Setting::Clone(void* pArg)
{
	CLogoBT_Setting*		pInstance = new CLogoBT_Setting(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Setting"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBT_Setting::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
