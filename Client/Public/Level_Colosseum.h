#pragma once

#include "Client_Defines.h"

#include "LandObject.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Colosseum final : public CLevel
{
private:
	CLevel_Colosseum(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Colosseum() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT LoadMap(const wstring& _strMapPath);
	

	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc);
	HRESULT Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc);
	HRESULT Ready_Layer_Pet(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

	HRESULT Ready_Layer_MainUI();

private:
	bool m_bChangeLevel = false;
	bool m_bScriptStart = false;
	_float m_fLevelChangeTime = 0.f;
	
private:
	void OpeningScript();

public:
	static CLevel_Colosseum* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END
