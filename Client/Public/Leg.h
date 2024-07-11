#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "RenderingObject.h"

BEGIN(Client)

class CLeg :
    public CRenderingObject
{
protected:
	CLeg(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLeg(const CLeg& Prototype);
	virtual ~CLeg() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_State(CFSM::OBJSTATE _eState) override;
	virtual HRESULT	Add_RenderObject(class CGameObject* pRenderObject) override;

private:
	CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	virtual 	HRESULT SetUp_FSM();

public:
	static CLeg* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
