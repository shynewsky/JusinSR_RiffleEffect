#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;

END

BEGIN(Client)

class CChaos final : public CEnemy
{
public:
	enum CHAOS_STATE
	{
		CHAOS_SPAWN,
		CHAOS_IDLE,
		CHAOS_WALK,
		CHAOS_MOVE,
		CHAOS_ATTACKA,
		CHAOS_ATTACKB,
		CHAOS_ATTACKC,
		CHAOS_ATTACKD,
		CHAOS_DEATH,
		CHAOS_END,
	};
private:
	CChaos(LPDIRECT3DDEVICE9 pGraphic_Device);
	CChaos(const CChaos& Prototype);
	virtual ~CChaos() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_State(CHAOS_STATE eState) { 
		m_eState = eState; 
		m_iPatternACount = 0;
	}

private:
	void Update_Spawn(_float fTimeDelta);
	void Update_Idle(_float fTimeDelta);
	void Update_Walk(_float fTimeDelta);
	void Update_Move(_float fTimeDelta);
	void Update_AttackA(_float fTimeDelta);
	void Update_AttackB(_float fTimeDelta);
	void Update_AttackC(_float fTimeDelta);
	void Update_AttackD(_float fTimeDelta);
	void Update_Death(_float fTimeDelta);

public:
	virtual void IsPicked(_float3* pOutPos, _float* pDist) override;

private:
	CTexture*				m_pTextureCom[CHAOS_END] = {nullptr};
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CHAOS_STATE				m_eState = CHAOS_SPAWN;
	_float4x4				m_CollisionMatrix;
	_float3					m_vCenterPos = _float3(50.f, 5.f, 50.f);
	_float3					m_vDir = {};
	_float					m_fHeight = 10.f;
	_float					m_fSumTime = 0.f;
	_float					m_fAttackTime = 2.f;
	_float					m_fAfterImageTime = 0.f;



	int						m_iPatternACount = 0;
	int						m_iPatternBCount = 0;
	int						m_iPatternCCount = 0;
	int						m_iPatternDCount = 0;
	int						m_iPrevPattern = 2;
	bool					m_bOnWater = false;
	bool					m_bDir = false;
	bool					m_bEndTex = false;
	bool					m_bPatternFirst = false;

public:
	static CChaos* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END