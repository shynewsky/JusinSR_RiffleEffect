#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CScrollTerrain final : public CGameObject
{
private:
	CScrollTerrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CScrollTerrain(const CScrollTerrain& Prototype);
	virtual ~CScrollTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = { nullptr };
	_float4x4				m_ScrollMatrix;
	_float					scrollOffset = 0.f;
	_float					m_fDir = 0.001f;

private:
	HRESULT Ready_Components();

public:
	static CScrollTerrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END