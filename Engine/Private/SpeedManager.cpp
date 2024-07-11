#include "SpeedManager.h"

CSpeedManager::CSpeedManager()
{
}

CSpeedManager::~CSpeedManager()
{
}

void CSpeedManager::Initialize()
{
}

void CSpeedManager::Update(_float _fTimeDelta)
{
	if (0 >= m_fTimer)
	{
		m_fCurSpeed = m_fCommonSpeed;
		return;
	}
	m_fTimer -= _fTimeDelta;
}

void CSpeedManager::Change_Speed(_float _timeer)
{
	m_fTimer = _timeer;
	m_fCurSpeed = m_fChangedSpeed;
}

CSpeedManager* CSpeedManager::Create()
{
	return new CSpeedManager();
}

void CSpeedManager::Free()
{
	__super::Free();
}
