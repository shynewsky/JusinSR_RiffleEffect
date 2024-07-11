#include "stdafx.h"
#include "Enemy_SM.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"

CEnemy_SM::CEnemy_SM(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{ pGraphic_Device }
{
}

CEnemy_SM::CEnemy_SM(const CEnemy_SM& Prototype)
	:CLandObject{ Prototype }
{
}

HRESULT CEnemy_SM::Initialize(void* pArg)
{

	EnemySM_Desc* desc = static_cast<EnemySM_Desc*>(pArg);

	m_pTerrainVIBuffer = desc->pTerrainVIBuffer;
	Safe_AddRef(m_pTerrainVIBuffer);

	m_pTerrainTransform = desc->pTerrainTransform;
	Safe_AddRef(m_pTerrainTransform);

	m_bLeft = desc->Left;

	return S_OK;
}

void CEnemy_SM::Free()
{
	__super::Free();
	//CFSM::Destroy(m_pFSM);
	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	
}
