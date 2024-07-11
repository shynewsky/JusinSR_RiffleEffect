#include "stdafx.h"
#include "..\Public\LogoBT_Practice.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLogoBT_Practice::CLogoBT_Practice(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CButton{ pGraphic_Device }
{
}

CLogoBT_Practice::CLogoBT_Practice(const CLogoBT_Practice & Prototype)
	: CButton{ Prototype }
{
}

CLogoBT_Practice * CLogoBT_Practice::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoBT_Practice*		pInstance = new CLogoBT_Practice(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Practice"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLogoBT_Practice::Clone(void* pArg)
{
	CLogoBT_Practice*		pInstance = new CLogoBT_Practice(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Practice"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBT_Practice::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
