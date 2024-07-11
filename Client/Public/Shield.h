#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Sphere;
class CVIBuffer_Terrain;
END


class CShield :
    public CBlendObject
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
    CShield(LPDIRECT3DDEVICE9 pGraphic_Device);
    CShield(const CShield& Prototype);
    virtual ~CShield() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual int Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void KillSwitch();

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pTransformPlayer = { nullptr };
	CVIBuffer_Sphere* m_pVIBufferCom = { nullptr };

private:
	_float m_DeadCount;
	LEVELID m_eCurLevel;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();


public:
	static CShield* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
