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

class CGreek_Column :public CBlendObject
{
public:
	typedef struct
	{
		_float3 vPos;	// 위치
		_float3 vScale;	// 크기
		_float3 vDir;	// 이동방향
		_float fMaxHeight; // 최대높이
	}BULLET_DESC;

private:
	CGreek_Column(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGreek_Column(const CBlendObject& Prototype);
	virtual ~CGreek_Column() = default;

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
	CVIBuffer_Cylinder* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };

	_float4x4	m_CollisionMatrix;
	_float		m_fCollisionTime = 0.f;

	_float3* m_pPos;
	_float3	m_vDIr;
	_float m_fMaxHeight;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	D3DXVECTOR3				m_vInertia = { 0.f, 0.f, 0.f };

public:
	static CGreek_Column* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

