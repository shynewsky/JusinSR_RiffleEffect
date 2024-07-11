#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END;

BEGIN(Client)

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

public:
	enum CAMID {CAM1_ORBIT, CAM2_FOLLOW, CAM3_ZOOM, CAM_END};

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Initialize();
	void Update();
	HRESULT Render();

public:
	void Set_GameInstance(CGameInstance* pGameInstance)
	{ 
		m_pGameInstance = pGameInstance;
	}

	bool Get_isTPS() { return m_bTPS; }

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	bool m_bTPS = false;

public:
	static CCamera_Manager* Create();
	virtual void Free() override;

};

END;