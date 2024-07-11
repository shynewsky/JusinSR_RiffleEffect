#include "..\Public\Picking_Manager.h"
#include "Component.h"
#include "Transform.h"
#include "GameInstance.h"
#include <iostream>

CPicking_Manager::CPicking_Manager()
{
}

HRESULT CPicking_Manager::Initialize()
{
	return S_OK;
}

HRESULT CPicking_Manager::Add_PickingGroup(PICKING_GROUP ePickingGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[ePickingGroup].push_back(pGameObject);
	//Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CPicking_Manager::Reset_PickingGroup()
{
	for (_uint i = 0; i < PICK_END; ++i)
	{
		for (auto& pGameObject : m_GameObjects[i])
		{
			if (nullptr != pGameObject)
			{
				//Safe_Release(pGameObject);
			}
		}
		m_GameObjects[i].clear();
	}

	return S_OK;
}
void  CPicking_Manager::Picking_Check_Group(PICKING_GROUP eGroup)
{

	CVIBuffer* pVIBuffer = nullptr;
	CTransform* pTransform = nullptr;
	vector<CGameObject*> m_vecGameObject;
	_float3 vPickPos = {};
	_float fMindist = 9999.f;
	for (auto& iter : m_GameObjects[eGroup])
	{
		if (iter == nullptr || iter->Get_Dead() == true)
			continue;

		pVIBuffer = static_cast<CVIBuffer*>(iter->Find_Component(TEXT("Com_VIBuffer")));
		pTransform = static_cast<CTransform*>(iter->Find_Component(TEXT("Com_Transform")));

		if (pVIBuffer == nullptr || pTransform == nullptr)
			return;

		_float3 pOutPos = {};
		_float pDist = {};

		if (pVIBuffer->isPicking(pTransform->Get_WorldMatrix(), &pOutPos, &pDist))
		{
			if (fMindist > pDist)
			{
				m_vecGameObject.push_back(iter);
				vPickPos = pOutPos;
				fMindist = pDist;
			}
		}
	}

	if (!m_vecGameObject.empty())
	{
		m_vecGameObject.back()->IsPicked(&vPickPos, &fMindist);
	}
	return;
}



CPicking_Manager* CPicking_Manager::Create()
{
	CPicking_Manager* pInstance = new CPicking_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPicking_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking_Manager::Free()
{
	for (_uint i = 0; i < PICK_END; ++i)
		m_GameObjects[i].clear();
}