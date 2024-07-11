#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Sphere;
class CCollider_Cube;
END

BEGIN(Client)

class CBullet_Sphere :public CBlendObject
{
public:
	typedef struct
	{
		_float3 vPos;
		_float3 vLook;
	}BULLET_DESC;

private:
	CBullet_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBullet_Sphere(const CBlendObject& Prototype);
	virtual ~CBullet_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Sphere* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };

	_float4x4	m_CollisionMatrix;
	_float3		m_vMyLook;
	_float		m_fCollisionTime = 0.f;
	_float3*	m_pPos;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	D3DXVECTOR3				m_vInertia = { 0.f, 0.f, 0.f };


public:
	static CBullet_Sphere* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

