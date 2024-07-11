#include "stdafx.h"
#include "../Public/Camera_Manager.h"

#include "MainApp.h"
#include "../Public/Camera.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
{
}

HRESULT CCamera_Manager::Initialize()
{
	return S_OK;
}

void CCamera_Manager::Update()
{
}

HRESULT CCamera_Manager::Render()
{
	return S_OK;
}

CCamera_Manager* CCamera_Manager::Create()
{
	return nullptr;
}

void CCamera_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
