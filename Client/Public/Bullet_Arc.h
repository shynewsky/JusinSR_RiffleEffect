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

class CBullet_Arc :
	public CLandObject
{
public:
	typedef struct 
	{
		_float3 vPos;
		_float3 vTargetPos;

		CVIBuffer_Terrain* pTerrainVIBuffer;
		CTransform* pTerrainTransform;
	}BULLETARC_DESC;

private:
	CBullet_Arc(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBullet_Arc(const CBullet_Arc& Prototype);
	virtual ~CBullet_Arc() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void KillSwitch() override { m_bDead = true; }

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };

	_float4x4	m_CollisionMatrix;
	_float3		m_vInitialLook;
	_float3		m_vTargetPos;
	_float		m_fCollisionTime = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	D3DXVECTOR3				m_vInertia = { 0.f, 0.f, 0.f };

public:
	static CBullet_Arc* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END