#include "stdafx.h"
#include "SpawningPool.h"
#include "GameInstance.h"
#include "Alien.h"
#include "Big_worm.h"
#include "Red.h"
#include "Chaos.h"
#include "FlyingMonster.h"
#include "PracticeTarget.h"
#include "Enemy.h"

CSpawningPool::CSpawningPool(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{pGraphic_Device}
{

}

CSpawningPool::CSpawningPool(const CSpawningPool& Prototype)
	:CLandObject(Prototype)
{

}

HRESULT CSpawningPool::Initialize(void* pArg)
{
	m_EnemyDesc = *static_cast<ENEMY_DESC*>(pArg);
	m_vPos = m_EnemyDesc.vDir;
	m_eCurLevel = m_EnemyDesc.eLevel;
	m_EnemyDesc.fScale = { 1.f,1.f,1.f };
	m_EnemyDesc.iMaxHP = 1;
	return S_OK;
}

HRESULT CSpawningPool::Spawn_Red(_float3 vPos)
{
	m_EnemyDesc.vDir = vPos;
	m_EnemyDesc.iMaxHP = 4;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Red"), &m_EnemyDesc)))
	{
		MSG_BOX(TEXT("Failed To Clone CRed!_SpawningPool"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpawningPool::Spawn_Blue(_float3 vPos)
{
	m_EnemyDesc.vDir = vPos;
	m_EnemyDesc.iMaxHP = 7;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Blue"), &m_EnemyDesc)))
	{
		MSG_BOX(TEXT("Failed To Clone CBlue!_SpawningPool"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpawningPool::Spawn_Ailen(_float3 vPos)
{
	m_EnemyDesc.vDir = vPos;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Alien"), &m_EnemyDesc)))
	{
		MSG_BOX(TEXT("Failed To Clone CAilen!_SpawningPool"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpawningPool::Spawn_Alien_Vertical(_float3 vPos)
{
	m_EnemyDesc.vDir = vPos;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_ALien_Vertical"), &m_EnemyDesc)))
	{
		MSG_BOX(TEXT("Failed To Clone CAilen_Vertical!_SpawningPool"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpawningPool::Spawn_Big_worm(_float3 vPos)
{
	m_EnemyDesc.vDir = vPos;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_BigWorm"), &m_EnemyDesc)))
	{
		MSG_BOX(TEXT("Failed To Clone CAilen!_SpawningPool"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpawningPool::Spawn_Chaos(_float3 vPos)
{

	MSG_BOX(TEXT("개발중 : 이거 안쓸거같긴 한데"));
	return S_OK;
}

HRESULT CSpawningPool::Spawn_FlyingMonster(_float3 vPos)
{

	m_EnemyDesc.vDir = vPos;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_FlyingMonster"), &m_EnemyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpawningPool::Spawn_PracticeTarget(_float3 vPos)
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_PracticeTarget"), vPos)))
	{
		MSG_BOX(TEXT("Failed To Spawn PracticeTarget_SpawningPool"));
		return E_FAIL;
	}
	return S_OK;
}

CGameObject* CSpawningPool::Clone(void* pArg)
{
	return nullptr;
}

void CSpawningPool::Free()
{
	__super::Free();
}
