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

class CFell final : public CEnemy
{
public:
	enum FELL_STATE
	{
		FELL_SPAWN,
		FELL_IDLE,
		FELL_BURROW_START,
		FELL_BURROW,
		FELL_BURROW_END,
		FELL_DASH_START,
		FELL_DASH_END,
		FELL_DEATH,
		FELL_END,
	};
private:
	CFell(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFell(const CFell& Prototype);
	virtual ~CFell() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_State(FELL_STATE eState) { 
		m_eState = eState; 
	}

private:
	void Update_Spawn(_float fTimeDelta);
	void Update_Idle(_float fTimeDelta);
	void Update_Burrow_Start(_float fTimeDelta);
	void Update_Burrow(_float fTimeDelta);
	void Update_Burrow_End(_float fTimeDelta);
	void Update_Dash_Start(_float fTimeDelta);
	void Update_Dash_End(_float fTimeDelta);
	void Update_Death(_float fTimeDelta);

public:
	virtual void IsPicked(_float3* pOutPos, _float* pDist) override;

private:
	CTexture*				m_pTextureCom[FELL_END] = {nullptr};
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	FELL_STATE				m_eState = FELL_SPAWN;
	_float4x4				m_CollisionMatrix;
	_float3					m_vCenterPos = _float3(50.f, 5.f, 50.f);
	_float3					m_vDir = {};
	_float3					m_vDirLeft, m_vDirRight;
	_float					m_fHeight = 10.f;
	_float					m_fSumTime = 0.f;
	_float					m_fAttackTime = 2.f;
	_float					m_fAngle = 0.f;



	_int						m_iBurrowCount = 0;
	_int						m_iDashCount = 0;
	bool					m_bPattern = false;

public:
	static CFell* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END