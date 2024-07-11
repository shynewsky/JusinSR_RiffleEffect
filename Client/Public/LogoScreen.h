#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CLogoScreen final : public CGameObject
{
private:
	CLogoScreen(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoScreen(const CLogoScreen& Prototype);
	virtual ~CLogoScreen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_LogoBG();
	HRESULT Ready_Layer_LogoBT();

public:
	static CLogoScreen* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END