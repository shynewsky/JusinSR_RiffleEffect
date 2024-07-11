#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "RenderingObject.h"

BEGIN(Client)

class WeaponEquiped :
	public CRenderingObject
{
protected:
	WeaponEquiped(LPDIRECT3DDEVICE9 pGraphic_Device);
	WeaponEquiped(const WeaponEquiped& Prototype);
	virtual ~WeaponEquiped() = default;

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
	void Change_EquipingWeapon(int iWeaponNum);
	void Shoot();

private:
	CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };

private:
	HRESULT	Ready_Components();
	HRESULT	SetUp_RenderState();
	HRESULT	Reset_RenderState();
	virtual HRESULT SetUp_FSM();

	_float		m_fShootDegree = 0.f;

public:
	static WeaponEquiped* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
