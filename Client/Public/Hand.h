#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "RenderingObject.h"

BEGIN(Client)

class CHand :
    public CRenderingObject
{
protected:
	CHand(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHand(const CHand& Prototype);
	virtual ~CHand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Reset_Frame() override;
	void	ChangeWeapon();
	void	ReloadMotion();
	virtual HRESULT	Add_RenderObject(class CGameObject* pRenderObject) override;

private:
	CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	virtual 	HRESULT SetUp_FSM();
public:
	static CHand* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
