#pragma once
#include "SpawningPool.h"

class CPool_Collosseum :
	public CSpawningPool
{
private:
	CPool_Collosseum(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPool_Collosseum(const CPool_Collosseum& Prototype);
	virtual ~CPool_Collosseum() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual int Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

private:
	_float	m_fSpawnCoolTime = { 2.f };
	_float	m_fDefaultSpawnTime = { 5.f };
	_int	m_iCurWave = { 0 };
	_int	m_iAct = { 1 }; //대기중인 act 넘버
	_bool	m_bStartStage = { false };
	list<CGameObject*>* m_pListMonster = {nullptr};
	CGameObject* m_pSearch_Scan = { nullptr };
	_bool	m_bBgmOn = { false };

private:
	void Wave1();
	void Wave2();
	void Wave3();
	void Wave4();
	void Wave5();
	void DefaultSpawn();
	void Act1();	//첫몹 다 잡은 시점의 이벤트
	void Act2();	//전투
	void Act3();	//탈출
	void Act4();	//탈출을 위한 방화벽 해제

public:
	static CPool_Collosseum* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
