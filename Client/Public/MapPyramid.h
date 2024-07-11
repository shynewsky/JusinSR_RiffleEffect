#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Pyramid;
END

BEGIN(Client)



class CMapPyramid final : public CGameObject
{

private:
	CMapPyramid(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapPyramid(const CMapPyramid& Prototype);
	virtual ~CMapPyramid() = default;

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
	CVIBuffer_Pyramid*			m_pVIBufferCom = { nullptr };


private:
	HRESULT Ready_Components();


public:
	static CMapPyramid* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END