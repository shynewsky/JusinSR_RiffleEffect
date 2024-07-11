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

class CBeam_Cylinder :public CBlendObject
{
public:
	typedef struct
	{
		_float3 vStartPos;
		_float3 vEndPos;
	}BULLET_DESC;

private:
	CBeam_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBeam_Cylinder(const CBlendObject& Prototype);
	virtual ~CBeam_Cylinder() = default;

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
	CGameObject* m_pPlayer = { nullptr };
	

private:
	_float4x4	m_CollisionMatrix;
	_float3		m_vMyLook;
	_float		m_fCollisionTime = 0.f;
	_float		m_fSumTime = 1.f;
	D3DXVECTOR3	m_vInertia = { 0.f, 0.f, 0.f };

private:
	_float3 m_vDescLook = {};
	_float	m_fDegreeAngle = 0.f;
	DWORD	m_dwTime = GetTickCount();
	bool	m_bRotate = false;

private: //충돌 계산용
	_float3 m_vStart;
	_float3 m_vEnd;

private:
	_float m_fFrame = {};

private:
	_bool Check_DeltaPlayer();//충돌판정 + 후처리
	void GronudEffect();

public:
	static CBeam_Cylinder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

