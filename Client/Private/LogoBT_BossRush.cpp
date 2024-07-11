#include "stdafx.h"
#include "..\Public\LogoBT_BossRush.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLogoBT_BossRush::CLogoBT_BossRush(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CButton{ pGraphic_Device }
{
}

CLogoBT_BossRush::CLogoBT_BossRush(const CLogoBT_BossRush & Prototype)
	: CButton{ Prototype }
{
}

CLogoBT_BossRush * CLogoBT_BossRush::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoBT_BossRush*		pInstance = new CLogoBT_BossRush(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_BossRush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLogoBT_BossRush::Clone(void* pArg)
{
	CLogoBT_BossRush*		pInstance = new CLogoBT_BossRush(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBT_BossRush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBT_BossRush::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
