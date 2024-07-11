#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

BEGIN(Client)
class CGrenade :
    public CLandObject
{
public:
	typedef struct 
	{
		_float3 vPos;
		_float3 vLook;
		_float fPower;

		CVIBuffer_Terrain* pTerrainVIBuffer;
		CTransform* pTerrainTransform;
		LEVELID eCurLevel;
	}GRENADE_DESC;

private:
	CGrenade(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGrenade(const CGrenade& Prototype);
	virtual ~CGrenade() = default;

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
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };

	_float4x4	m_CollisionMatrix;
	_float3		m_vInitialLook;
	_float3		m_vLook;
	_float		m_fCollisionTime = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	D3DXVECTOR3				m_vInertia = { 0.f, 0.f, 0.f };

public:
	static CGrenade* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END