#pragma once
#include "SpawningPool.h"
class CPool_SM :
    public CSpawningPool
{
public:
	typedef struct EnemySM_Desc
	{
		CVIBuffer_Terrain* pTerrainVIBuffer = { nullptr };
		CTransform* pTerrainTransform = { nullptr };
		_float3 fScale = { 1.f, 1.f, 1.f };
		_float3 vPos;
		_bool Left = true; //진행방향
	};

private:
	CPool_SM(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPool_SM(const CPool_SM& Prototype);
	virtual ~CPool_SM() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual int Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

public:
	HRESULT Spawn_WaveShot(_float3 _vPos, _bool _bLeft);
	HRESULT Spawn_Dagger(_float3 _vPos, _bool _bLeft);
	HRESULT Spawn_Slime(_float3 _vPos, _bool _bLeft);
	HRESULT Spawn_ConveyerBelt(_float3 _vPos, _bool _bLeft);
	HRESULT Spawn_Kumiho(_float3 _vPos, _bool _bLeft);
	HRESULT Spawn_KumihoRunning(_float3 _vPos, _bool _bLeft);
	HRESULT Spawn_RazorLine(_float3 _vPos, _bool _bLeft);
	HRESULT Spawn_RazorLine_Vertical(_float3 _vPos, _bool _bLeft);

private:
	_float m_fCooltime[30];
	_float m_fRecordCooltime[30];
	EnemySM_Desc m_SM_Desc;
	_float m_fOctoCooltime = { 0.f };
	_float m_fOctoVerCooltime[5];

private:
	void SpawnLine();
	_bool CheckLine(_int _iLineNum);

public:
	static CPool_SM* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
