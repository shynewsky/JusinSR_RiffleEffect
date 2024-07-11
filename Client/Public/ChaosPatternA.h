#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "FSM.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;

END

BEGIN(Client)

class CChaosPatternA final : public CLandObject
{
	enum PATTERN_STATE
	{
		PATTERN_SPAWN,
		PATTERN_IDLE,
		PATTERN_CHARGE,
		PATTERN_DEATH,
		PATTERN_END,
	};
private:
	CChaosPatternA(LPDIRECT3DDEVICE9 pGraphic_Device);
	CChaosPatternA(const CChaosPatternA& Prototype);
	virtual ~CChaosPatternA() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Update_Spawn(_float fTimeDelta);
	void Update_Idle(_float fTimeDelta);
	void Update_Charge(_float fTimeDelta);
	void Update_Death(_float fTimeDelta);

public:

private:
	CTexture*				m_pTextureCom[PATTERN_END] = {nullptr};
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };
	bool					m_bRayzerSet = false;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	PATTERN_STATE			m_eState = PATTERN_SPAWN;
	CGameObject*			m_pRayzer = { nullptr };
	CGameObject*			m_pInterection = { nullptr };
	CGameObject*			m_pIndicator = { nullptr };
	_float4x4				m_CollisionMatrix;
	_float					m_fSumTime = 0.f;
	_float					m_fAttackTime = 2.f;
	_uint					m_iHP = 200;
	bool					m_bDir = false;
	bool					m_bEndTex = false;

public:
	static CChaosPatternA* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END