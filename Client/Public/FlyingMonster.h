#pragma once

#include "Client_Defines.h"
#include "FSM.h"
#include "Enemy.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;

END

BEGIN(Client)

class CFlyingMonster final : public CEnemy
{
	enum MY_STATE
	{
		STATE_IDLE,
		STATE_ATTACK,
		STATE_DEATH,
		STATE_END,
	};

private:
	CFlyingMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFlyingMonster(const CFlyingMonster& Prototype);
	virtual ~CFlyingMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Update_Idle(_float fTimeDelta);
	void Update_Attack(_float fTimeDelta);
	void Update_Death(_float fTimeDelta);

public:
	virtual void IsPicked(_float3* pOutPos, _float* pDist) override;

private:
	CTexture*				m_pTextureCom[STATE_END] = {nullptr};
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	MY_STATE				m_eState = STATE_IDLE;
	_float4x4				m_CollisionMatrix;
	bool					m_bDir = false;
	bool					m_bFistDead = false;
	_float					m_fSumTime = 0.f;
	_float					m_fAttackTime = 2.f;
	_int					m_iHP = 4;
	float scrollOffset = 0.0f;
	D3DXMATRIX matTex;
public:
	static CFlyingMonster* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END