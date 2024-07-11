#include "..\Public\Collider_Manager.h"
#include "Component.h"
#include "Transform.h"
#include "Collider_Cube.h"
#include "GameInstance.h"

CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Initialize()
{
	return S_OK;
}

HRESULT CCollider_Manager::Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[eCollisionGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CCollider_Manager::Reset_ColliderGroup()
{
	for (_uint i = 0; i < COLLISION_GROUPEND; ++i)
	{
		for (auto& pGameObject : m_GameObjects[i])
		{
			if (nullptr != pGameObject)
			{
				Safe_Release(pGameObject);
			}
		}
		m_GameObjects[i].clear();
	}

	return S_OK;
}
_bool  CCollider_Manager::Collision_Check_Group(COLLISION_GROUP eGroup, class CGameObject* pGameObject, COLLISION_TYPE eCollisionType, _float3* pOutDistance )
{
	CComponent* pMyCollider = nullptr;
	CComponent* pOther = nullptr;

	for (auto& iter : m_GameObjects[eGroup])
	{
		switch (eCollisionType)
		{
		case Engine::CCollider_Manager::COLLSIION_BOX:
				pMyCollider = (CCollider_Cube*)pGameObject->Find_Component(TEXT("Com_Collider_Cube"));
				pOther = (CCollider_Cube*)iter->Find_Component(TEXT("Com_Collider_Cube"));
			if (pOther == nullptr)
				continue;
			if (true == ((static_cast<CCollider_Cube*>(pMyCollider)->Collision_Check((CCollider_Cube*)pOther, pOutDistance))))
				return true;
			break;
		default:
			break;
		}
	}

	return false;
}

_bool CCollider_Manager::Collision_Check_Group_Multi(COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, CGameObject * pDamageCauser, COLLISION_TYPE eCollisionType)
{
	CComponent* pMyCollider = nullptr;
	CComponent* pOther = nullptr;

	for (auto& iter : m_GameObjects[eGroup])
	{
		switch (eCollisionType)
		{
		case Engine::CCollider_Manager::COLLSIION_BOX:
			pMyCollider = (CCollider_Cube*)pDamageCauser->Find_Component(TEXT("Com_Collider_Cube"));
			pOther = (CCollider_Cube*)iter->Find_Component(TEXT("Com_Collider_Cube"));
			if (pOther == nullptr)
				continue;

			if (true == (static_cast<CCollider_Cube*>(pMyCollider)->Collision_Check((CCollider_Cube*)pOther)))
				vecDamagedObj.push_back(iter);
			break;
		default:
			break;
		}
	}
	
	if (vecDamagedObj.size() > 0)
	{
		return true;
	}
	else 
	{
		return false;
	}
}


CCollider_Manager* CCollider_Manager::Create()
{
	CCollider_Manager* pInstance = new CCollider_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Manager::Free()
{
	for (_uint i = 0; i < COLLISION_GROUPEND; ++i)
		m_GameObjects[i].clear();
}