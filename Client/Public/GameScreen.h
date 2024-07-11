#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Image.h"
#include "Player.h"
#include "GattoIcon.h"
#include "HeartIcon.h"

//#include "GunUsing.h"
//#include "GunSlot1_Rambo.h"
//#include "GunSlot2_Sparrow.h"
//#include "GunSlot3_FlameShaker.h"

BEGIN(Client)

class CGameScreen final : public CGameObject
{
public:
	typedef struct
	{
		LEVELID eCurLevel;
	}GAMEUI_DESC;

private:
	CGameScreen(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameScreen(const CGameScreen& Prototype);
	virtual ~CGameScreen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_GameBG();
	HRESULT Ready_Layer_Hearts();
	HRESULT Ready_Layer_Gauge();
	HRESULT Ready_Layer_Frame();
	HRESULT Ready_Layer_Gun();
	HRESULT Ready_Layer_SkillCool();
	HRESULT Ready_Layer_Bullet();
	HRESULT Ready_Layer_Immotal();

private:
	void Control_Heart();
	void Control_Gun();

private:
	GAMEUI_DESC m_tGameUIInfo;

#pragma region GattoIcon, HeartIcon

private:
	CPlayer* m_pPlayer;
	CGattoIcon* m_pGattoIcon;
	list<CHeartIcon*> m_HeartList;

private:
	int m_iPreHeart;
	int m_iCurHeart;

#pragma endregion

#pragma region Gun,GunSlot Icon
	
private:
	class CGunSlotSelect* m_pGunSlotSelect;
	class CGunUsing* m_pGunUsing;
	class CGunSlot1_Rambo* m_pGunSlot1;
	class CGunSlot2_Sparrow* m_pGunSlot2;
	class CGunSlot3_FlameShaker* m_pGunSlot3;

#pragma endregion

public:
	static CGameScreen* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END