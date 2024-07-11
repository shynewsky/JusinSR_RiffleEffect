#pragma once
#include "LandObject.h"

class CSpawningPool abstract:
    public CLandObject
{
public:
	typedef struct
	{
		CVIBuffer_Terrain* pTerrainVIBuffer = { nullptr };
		CTransform* pTerrainTransform = { nullptr };
		LEVELID					eLevel = { LEVEL_GAMEPLAY };

		_float3  fScale = { 1.f,1.f,1.f }; //xyz 스케일링값
		_float3 vDir;           //생성위치
		_int    iMaxHP = 0;
	}ENEMY_DESC;
	typedef struct
	{
		CVIBuffer_Terrain* pTerrainVIBuffer = { nullptr };
		CTransform* pTerrainTransform = { nullptr };
		LEVELID					eLevel = { LEVEL_GAMEPLAY };
	}LANDOBJECT_DESC;

protected:
    CSpawningPool(LPDIRECT3DDEVICE9 pGraphic_Device);
    CSpawningPool(const CSpawningPool& Prototype);
    virtual ~CSpawningPool() = default;

public:
	virtual HRESULT Initialize_Prototype() = 0;
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta) = 0;
	virtual int Update(_float fTimeDelta) = 0;
	virtual void Late_Update(_float fTimeDelta) = 0;

public:
	HRESULT Spawn_Red(_float3 vPos);
	HRESULT Spawn_Blue(_float3 vPos);
	HRESULT Spawn_Ailen(_float3 vPos);
	HRESULT Spawn_Alien_Vertical(_float3 vPos);
	HRESULT Spawn_Big_worm(_float3 vPos);
	HRESULT Spawn_Chaos(_float3 vPos);
	HRESULT Spawn_FlyingMonster(_float3 vPos);
	HRESULT Spawn_PracticeTarget(_float3 vPos);

protected:
	LEVELID m_eCurLevel;
	_float3 m_vPos;
	ENEMY_DESC m_EnemyDesc;
	LANDOBJECT_DESC m_LandObjectDesc;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

