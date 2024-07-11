#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CAttackWay :
	public CBlendObject
{
public:
	typedef struct
	{
		_float fX;
		_float fZ;
		_float3 fDir;
	}ATTACKWAYDESC;
	
protected:
	CAttackWay(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAttackWay(const CAttackWay& Prototype);
	virtual ~CAttackWay() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	ATTACKWAYDESC m_tDesc;
	_float3 m_vDir = {};
	_float m_fSumTime = 0.f;
	_float m_fScale = 1.f;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CAttackWay* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
