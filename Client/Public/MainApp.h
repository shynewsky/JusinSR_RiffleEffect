#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT	Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	LPDIRECT3DDEVICE9		m_pGraphic_Device = { nullptr };

public:
	HRESULT Loading_Resource_For_LoadingLevel();

private:
	HRESULT Ready_Default_Setting();
	HRESULT Ready_Prototype_Component_Static();
	HRESULT Open_Level(LEVELID eStartLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END