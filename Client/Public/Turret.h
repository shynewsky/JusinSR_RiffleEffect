#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CTurret final : public CGameObject
{
	enum MY_STATE
	{
		STATE_SPAWN,
		STATE_IDLE,
		STATE_ATTACK,
		STATE_DEATH,
		STATE_END,
	};
private:
	CTurret(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTurret(const CTurret& Prototype);
	virtual ~CTurret() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Update_Spawn(_float fTimeDelta);
	void Update_Idle(_float fTimeDelta);
	void Update_Attack(_float fTimeDelta);
	void Update_Death(_float fTimeDelta);

public:
	virtual void IsPicked(_float3* pOutPos, _float* pDist) override;

private:
	CTexture*				m_pTextureCom[STATE_END] = {nullptr};
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	MY_STATE				m_eState = STATE_SPAWN;
	_float3					m_vAttackDir = {};
	_float					m_fSumTime = 0.f;
	_float					m_fAttackTime = 0.2f;
	_uint					m_iBulletCount = 0;
	_int					m_iHP = 5;
	_bool					m_bAttackStart = true;
	_bool					m_bDeadStart = true;


public:
	static CTurret* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END