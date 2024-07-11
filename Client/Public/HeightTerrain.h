#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CHeightTerrain final : public CGameObject
{
private:
	CHeightTerrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHeightTerrain(const CHeightTerrain& Prototype);
	virtual ~CHeightTerrain() = default;

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

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CHeightTerrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END