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

class CPracticeTarget final : public CLandObject
{
	enum TARGET_STATE
	{
		TS_STAND,
		TS_DOWN,
		TS_WAIT,
		TS_END,
	};
private:
	CPracticeTarget(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPracticeTarget(const CPracticeTarget& Prototype);
	virtual ~CPracticeTarget() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void IsPicked(_float3* pOutPos, _float* pDist) override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	TARGET_STATE			m_eTargetState = TS_STAND;
	_float4x4				m_CollisionMatrix;
	_float4x4				m_DownMatrix;
	_float3					m_OriginPos;
	_float3					m_ReStartPos;
	_float					m_fMaxX = 20.f;
	_float3					m_fSpeed = _float3(0.2f, 0.f, 0.f);
	_float					m_fMoveDistance = 0.f;
	_float					m_fDownTime = 0.f;
	bool					m_MyDir = false;


public:
	static CPracticeTarget* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END