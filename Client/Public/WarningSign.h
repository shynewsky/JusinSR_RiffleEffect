#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "RenderingObject.h"

BEGIN(Client)

class CWarningSign :
	public CRenderingObject
{
public:
	
protected:
	CWarningSign(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWarningSign(const CWarningSign& Prototype);
	virtual ~CWarningSign() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT	Add_RenderObject(class CGameObject* pRenderObject) override;

private:
	CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };
	_float m_fMaxSize = { 1.f };
	_float m_fTimer = { 1.f };
	_float m_fCurTimer = { 0.f };
	LEVELID m_eCurlevel;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	virtual 	HRESULT SetUp_FSM();

public:
	static CWarningSign* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
