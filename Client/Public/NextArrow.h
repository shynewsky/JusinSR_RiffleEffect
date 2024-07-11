#pragma once

#include "Client_Defines.h"
#include "UIOrthogonal.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;


END

BEGIN(Client)

class CNextArrow final : public CUIOrthogonal
{

private:
	CNextArrow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CNextArrow(const CNextArrow& Prototype);
	virtual ~CNextArrow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = {nullptr};
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	_float					m_fOriginYPos = 0.f;
	bool					m_bDir = true;


public:
	static CNextArrow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END