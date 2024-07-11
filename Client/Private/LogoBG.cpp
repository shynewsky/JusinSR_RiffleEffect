#include "stdafx.h"
#include "..\Public\LogoBG.h"

#include "GameInstance.h"

CLogoBG::CLogoBG(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage { pGraphic_Device }
{
}

CLogoBG::CLogoBG(const CLogoBG & Prototype)
	: CImage{ Prototype }
{
}

CLogoBG * CLogoBG::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoBG*		pInstance = new CLogoBG(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLogoBG::Clone(void* pArg)
{
	CLogoBG*		pInstance = new CLogoBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoBG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoBG::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
