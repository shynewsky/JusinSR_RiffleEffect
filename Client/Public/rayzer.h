#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CCollider_Cube;

END

BEGIN(Client)

class CRayzer final : public CGameObject
{
public:
	typedef struct
	{
		_float3			vStartPos;
	}RAYZER_DESC;
private:
	CRayzer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRayzer(const CRayzer& Prototype);
	virtual ~CRayzer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:

private:
	CTransform*				m_pTransformCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Ready_VIBuffer();
	HRESULT Update_VIBuffer();
	HRESULT Render_VIBuffer();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

protected: // 버텍스 버퍼 셋팅용
	LPDIRECT3DVERTEXBUFFER9		m_pVBuffer = nullptr;
private:
	_float4x4				m_CollisionMatrix;
	_float3					m_vStartPos = {};
	_float					m_fSumTime = 0.f;

public:
	static CRayzer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END