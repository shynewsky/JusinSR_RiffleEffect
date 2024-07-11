#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CCollider_Cube;
class CShader;
END

BEGIN(Client)

class CMapBox final : public CBlendObject
{
private:
	enum BLIND_STATE { BS_NONE, BS_BLIND, BS_END };
private:
	CMapBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapBox(const CMapBox& Prototype);
	virtual ~CMapBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void IsPicked(_float3* pOutPos, _float* pDist)override;

private:
	CTexture*				m_pTextureCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Cube*			m_pVIBufferCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };

	CShader*				m_pShaderCom = { nullptr };
	_float4x4				m_CollisionMatrix;

	BLIND_STATE				m_eBlindState = BS_NONE;

	D3DLIGHT9				m_pLight;
	_float3					m_LightDir;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();


public:
	static CMapBox* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END