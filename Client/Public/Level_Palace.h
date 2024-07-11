#pragma once

#include "Client_Defines.h"

#include "LandObject.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Palace final : public CLevel
{
private:
	CLevel_Palace(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Palace() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT LoadMap(const wstring& _strMapPath);
	

	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc);
	HRESULT Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc);
	HRESULT Ready_Layer_Pet(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

	HRESULT Ready_Layer_MainUI();

	bool		m_bChangeLevel = false;
	_float		m_fLevelChangeTime = 0.f;
	

public:
	static CLevel_Palace* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END
