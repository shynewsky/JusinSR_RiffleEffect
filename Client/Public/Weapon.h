#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "RenderingObject.h"

BEGIN(Client)

class CWeapon :
    public CRenderingObject
{
protected:
	CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_PlayerTransform(CTransform* _pTransform) { m_pPlayerTransform = _pTransform; }
	virtual void Set_State(CFSM::OBJSTATE _eState) override;
	virtual HRESULT	Add_RenderObject(class CGameObject* pRenderObject) override;
	void	Change_Weapon(int iWeaponNum);
private:
	CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	CFSM* m_pFSM;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	virtual 	HRESULT SetUp_FSM();
public:
	static CWeapon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
