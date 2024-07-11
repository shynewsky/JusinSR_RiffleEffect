#pragma once
#include "Base.h"

BEGIN(Engine)

class CSpeedManager :
	public CBase
{
private:
	CSpeedManager();
	virtual ~CSpeedManager();

public:
	void Initialize();
	void Update(_float _fTimeDelta);

public:
	_float Get_CurSpeed() { return m_fCurSpeed; }
	_float Get_Timer() { return m_fTimer; }
	void Set_ChangedSpeed(_float _Speed) { m_fChangedSpeed = _Speed; }
	void Change_Speed(_float _timeer);

private:
	_float m_fCommonSpeed = { 1.f };
	_float m_fChangedSpeed = { 1.f };
	_float m_fCurSpeed = { 1.f };
	_float m_fTimer = { 0.f };

public:
	static CSpeedManager* Create();
	virtual void Free();
};

END