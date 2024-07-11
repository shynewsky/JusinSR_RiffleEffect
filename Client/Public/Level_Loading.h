#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_LoadingBG();

public:
	HRESULT Loading();
	HRESULT Loading_Resource_For_LogoLevel();
	HRESULT Loading_Resource_For_GamePlayLevel();
	HRESULT Loading_Resource_For_ColosseumLevel();
	HRESULT Loading_Resource_For_ChaosLevel();
	HRESULT Loading_Resource_For_NeroLevel();
	HRESULT Loading_Resource_For_PalaceLevel();
	HRESULT Loading_Resource_For_MazeLevel();
	HRESULT Loading_Resource_For_SMLevel();

private:
	LEVELID				m_eNextLevelID = { LEVEL_END };
	HANDLE				m_hThread = {};
	CRITICAL_SECTION	m_CriticalSection = {};

	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");
	_bool				m_isFinished = { false };

public:
	static CLevel_Loading* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVELID eNextLevelID);
	virtual void Free() override;
};

END
