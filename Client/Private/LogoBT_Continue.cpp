#include "stdafx.h"
#include "..\Public\LogoBT_Continue.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLogoBT_Continue::CLogoBT_Continue(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CButton{ pGraphic_Device }
{
}

CLogoBT_Continue::CLogoBT_Continue(const CLogoBT_Continue & Prototype)
	: CButton{ Prototype }
{
}

CLogoBT_Continue * CLogoBT_Continue::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoBT_Continue*		pInstance = new CLogoBT_Continue(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Continue"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLogoBT_Continue::Clone(void* pArg)
{
	CLogoBT_Continue*		pInstance = new CLogoBT_Continue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_Continue"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBT_Continue::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
