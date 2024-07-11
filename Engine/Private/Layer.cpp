#include "..\Public\Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		if (pGameObject != nullptr)
			pGameObject->Priority_Update(fTimeDelta);

	return S_OK;
}

HRESULT CLayer::Update(_float fTimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter!= m_GameObjects.end();)
	{
		if ((*iter) != nullptr)
			if ((*iter)->Update(fTimeDelta) == OBJ_DEAD)
			{
				Safe_Release(*iter);
				iter = m_GameObjects.erase(iter);
			}
			else
			{
				++iter;
			}
	}
	
	return S_OK;
}

HRESULT CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		if (pGameObject != nullptr)
			pGameObject->Late_Update(fTimeDelta);

	return S_OK;
}

CComponent * CLayer::Find_Component(const _wstring & strComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag);	
}

CLayer * CLayer::Create()
{
	return new CLayer();
}

CGameObject* CLayer::Get_Object(_uint iIndex)
{
	if (m_GameObjects.size() <= iIndex)
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return *iter;
}

void CLayer::Clear_ObjectList()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}

