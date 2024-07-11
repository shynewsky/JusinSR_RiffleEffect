#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "FSM.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CChaosPatternC final : public CLandObject
{
	enum PATTERN_STATE
	{
		PATTERN_SPAWN,
		PATTERN_IDLE,
		PATTERN_CHARGE,
		PATTERN_EXPLOSION,
		PATTERN_DEATH,
		PATTERN_END,
	};

public:
	typedef struct 
	{
		_float3 vPos;
		_float3 vTargetPos;

		CVIBuffer_Terrain* pTerrainVIBuffer;
		CTransform* pTerrainTransform;
	}PATTERN_DESC;
private:
	CChaosPatternC(LPDIRECT3DDEVICE9 pGraphic_Device);
	CChaosPatternC(const CChaosPatternC& Prototype);
	virtual ~CChaosPatternC() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void IsPicked(_float3* pOutPos, _float* pDist) override;

	void Update_Spawn(_float fTimeDelta);
	void Update_Idle(_float fTimeDelta);
	void Update_Charge(_float fTimeDelta);
	void Update_Explosion(_float fTimeDelta);
	void Update_Death(_float fTimeDelta);

public:

private:
	CTexture*				m_pTextureCom[PATTERN_END] = {nullptr};
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	PATTERN_STATE			m_eState = PATTERN_SPAWN;
	_float4x4				m_CollisionMatrix;
	_float3					m_vStartPos = {};
	_float3					m_vArrivePos = {};
	_float3					m_fDirection = {};
	_float3					m_fVelocity = {};
	_float					m_fGravity = 9.8f;
	_float					m_fSumTime = 0.f;
	_uint					m_iHP = 5;
	bool					m_bDir = false;
	bool					m_bEndTex = false;

public:
	static CChaosPatternC* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END