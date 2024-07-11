#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CCollider_Cube;
END

BEGIN(Client)

class CBullet_Cube :public CBlendObject
{
public:
	typedef struct
	{
		_float3 vPos;
		_float3 vMoveDir;
		_float3 vLookDir;
	}BULLET_DESC;

private:
	CBullet_Cube(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBullet_Cube(const CBlendObject& Prototype);
	virtual ~CBullet_Cube() = default;

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
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };

	_float4x4	m_CollisionMatrix;
	_float3		m_vMyLook;
	_float		m_fCollisionTime = 0.f;
	_float		m_fSumTime = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	D3DXVECTOR3				m_vInertia = { 0.f, 0.f, 0.f };

public:
	static CBullet_Cube* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

