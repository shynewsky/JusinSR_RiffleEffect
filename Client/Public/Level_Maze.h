#pragma once

#include "Client_Defines.h"

#include "LandObject.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Maze final : public CLevel
{
private:
	CLevel_Maze(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Maze() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	
	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc);
	HRESULT Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc);
	HRESULT Ready_Layer_Pet(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

	HRESULT Ready_Layer_Effect();

	HRESULT Ready_Layer_MainUI();
	HRESULT Ready_Layer_CursorUI();

public:
	HRESULT LoadMap(const wstring& _strMapPath);

private:
	bool		m_bChangeLevel = false;
	bool		m_bSoundTurnOn = false;
	_float		m_fLevelChangeTime = 0.f;

private:
	void OpeningScript();

public:
	static CLevel_Maze* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END
