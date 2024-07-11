#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Pyramid;
class CCollider_Cube;
END

BEGIN(Client)

class CPull_Pyramid :public CBlendObject
{
public:
	typedef struct
	{
		_float3 vStartPos;
		_float3 vEndPos;
		CTransform* TransformPlayer;
	}BULLET_DESC;

private:
	CPull_Pyramid(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPull_Pyramid(const CBlendObject& Prototype);
	virtual ~CPull_Pyramid() = default;

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
	CVIBuffer_Pyramid* m_pVIBufferCom = { nullptr };
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

private:
	_float3 m_vNeroPos = {};
	_float3 m_vPlayerPos = {};
	CTransform* m_pTransformPlayer = { nullptr };

	_float3 m_vSideNormal = {};
	_float3 m_vFarNormal = {};

	_float3 m_fPyramidCenter = {};
	_float m_fPyramidHeight = {};

public:
	static CPull_Pyramid* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

