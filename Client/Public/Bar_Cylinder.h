#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cylinder;
class CCollider_Cube;
END

BEGIN(Client)
            
class CBar_Cylinder :public CBlendObject
{
public:
	typedef struct
	{
		_float3 vPos;
		_float3 vLookDir;
	}BULLET_DESC;

private:
	CBar_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBar_Cylinder(const CBlendObject& Prototype);
	virtual ~CBar_Cylinder() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Cylinder* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };

private:
	_float4x4	m_CollisionMatrix;
	_float3		m_vMyLook;
	_float		m_fCollisionTime = 0.f;
	D3DXVECTOR3	m_vInertia = { 0.f, 0.f, 0.f };

private:
	_float3 m_vDescLook = {};
	_float	m_fDegreeAngle = 0.f;
	DWORD	m_dwTime = GetTickCount();
	bool	m_bRotate = false;

public:
	static CBar_Cylinder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

