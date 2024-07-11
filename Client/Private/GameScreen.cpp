#include "stdafx.h"
#include "../Public/GameScreen.h"

#include "GameInstance.h"

#include "GameBG.h"
#include "GameFG.h"
#include "GattoIcon.h"
#include "HeartIcon.h"
#include "GunSlotSelect.h"
#include "GunUsing.h"
#include "GunSlot1_Rambo.h"
#include "GunSlot2_Sparrow.h"
#include "GunSlot3_FlameShaker.h"
#include "Gauge_Skill.h"
#include "Gauge_Gun.h"

CGameScreen::CGameScreen(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CGameScreen::CGameScreen(const CGameScreen& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CGameScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameScreen::Initialize(void* pArg)
{
	GAMEUI_DESC* pDesc = static_cast<GAMEUI_DESC*>(pArg);
	m_tGameUIInfo.eCurLevel = pDesc->eCurLevel;
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_tGameUIInfo.eCurLevel, TEXT("Layer_Player")));

	if (FAILED(Ready_Layer_GameBG()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Hearts()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Gauge()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Frame()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Gun()))
		return E_FAIL;
	if (FAILED(Ready_Layer_SkillCool()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Bullet()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Immotal()))
		return E_FAIL;
	
	return S_OK;
}

void CGameScreen::Priority_Update(_float fTimeDelta)
{
}

int CGameScreen::Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CGameScreen::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	Control_Heart();

	Control_Gun();
}

HRESULT CGameScreen::Render()
{
	return S_OK;
}

HRESULT CGameScreen::Ready_Layer_GameBG()
{
	CGameBG::IMAGE_DESC Desc1 = { g_iWinSizeX/2, g_iWinSizeY/2,  g_iWinSizeX, g_iWinSizeY
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GameBG")};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeBack"),
		TEXT("Prototype_GameObject_GameBG"), &Desc1)))
		return E_FAIL;

	CGattoIcon::IMAGE_DESC Desc2 = { 100.f, 60.f,  78.f, 68.f
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GattoIcon"),9, 10 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeBack"),
		TEXT("Prototype_GameObject_GattoIcon"), &Desc2)))
		return E_FAIL;

	m_pGattoIcon = static_cast<CGattoIcon*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeBack"))->back());

	return S_OK;
}

HRESULT CGameScreen::Ready_Layer_Hearts()
{
	CHeartIcon::IMAGE_DESC Desc = {};

	m_iPreHeart = m_iCurHeart = m_pPlayer->Get_HP();

	for (int i = 0; i < m_iCurHeart; i++)
	{
		Desc = { 190.f + (50.f * i), 72.f, 70.f, 70.f, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HeartIcon"),20, 8};

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeHeart"),
			TEXT("Prototype_GameObject_HeartIcon"), &Desc)))
			return E_FAIL;

		CHeartIcon* pHeartIcon = static_cast<CHeartIcon*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeHeart"))->back());
		m_HeartList.push_back(pHeartIcon);
	}

	return S_OK;
}

HRESULT CGameScreen::Ready_Layer_Gauge()
{
	CGauge_Skill::GAUGE_DESC Desc1 =
	{
		//263.f, 670.f		// 위치
		360.f, 50.f			// 위치
		, 185.f, 10.f		// 크기
		, 6.f				// MaxValue
		, m_pGameInstance->Find_Object(m_tGameUIInfo.eCurLevel, TEXT("Layer_Player"))
		, 0, 0, 0.f
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeCenter"),
		TEXT("Prototype_GameObject_Gauge_Skill"), &Desc1)))
		return E_FAIL;

	CGauge_Gun::GAUGE_DESC Desc2 =
	{
		//1055.f, 655.f		// 위치
		1150.f, 65.f		// 위치
		, 185.f, 10.f		// 크기
		, 3.f				// MaxValue
		, m_pGameInstance->Find_Object(m_tGameUIInfo.eCurLevel, TEXT("Layer_Player"))
		, 0, 0, 0.f
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeCenter"),
		TEXT("Prototype_GameObject_Gauge_Gun"), &Desc2)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameScreen::Ready_Layer_Frame()
{
	CGameFG::IMAGE_DESC Desc1 = { g_iWinSizeX / 2, g_iWinSizeY / 2,  g_iWinSizeX, g_iWinSizeY
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GameFG") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeFront"),
		TEXT("Prototype_GameObject_GameFG"), &Desc1)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameScreen::Ready_Layer_Gun()
{
	CGunSlotSelect::IMAGE_DESC Desc1 = { 1237.f, 425.f,  39.f, 36.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunSlotSelect") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"),
		TEXT("Prototype_GameObject_GunSlotSelect"), &Desc1)))
		return E_FAIL;

	m_pGunSlotSelect = static_cast<CGunSlotSelect*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"))->back());

	CGunSlot1_Rambo::IMAGE_DESC Desc3 = { 1235.f, 425.f,  47.f, 40.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunSlot1"), 4 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"),
		TEXT("Prototype_GameObject_GunSlot1"), &Desc3)))
		return E_FAIL;

	m_pGunSlot1 = static_cast<CGunSlot1_Rambo*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"))->back());
	
	CGunSlot2_Sparrow::IMAGE_DESC Desc4 = { 1235.f, 485.f,  47.f, 40.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunSlot2"), 4 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"),
		TEXT("Prototype_GameObject_GunSlot2"), &Desc4)))
		return E_FAIL;

	m_pGunSlot2 = static_cast<CGunSlot2_Sparrow*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"))->back());

	CGunSlot3_FlameShaker::IMAGE_DESC Desc5 = { 1235.f, 545.f,  47.f, 45.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunSlot3"), 4 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"),
		TEXT("Prototype_GameObject_GunSlot3"), &Desc5)))
		return E_FAIL;

	m_pGunSlot3 = static_cast<CGunSlot3_FlameShaker*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"))->back());

	CGunUsing::IMAGE_DESC Desc2 = { 1205.f, 635.f,  100.f, 90.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunUsing"), 6 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"),
		TEXT("Prototype_GameObject_GunUsing"), &Desc2)))
		return E_FAIL;

	m_pGunUsing = static_cast<CGunUsing*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_Gun"))->back());

	return S_OK;
}

HRESULT CGameScreen::Ready_Layer_SkillCool()
{
	CGattoIcon::IMAGE_DESC Desc2 = { 136.f, 650.f,  89.f, 78.f 
			, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkillCool"),
	(int)m_tGameUIInfo.eCurLevel, 0 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_SkillCool"),
		TEXT("Prototype_GameObject_SkillCool"), &Desc2)))
		return E_FAIL;

	Desc2 = { 136.f, 645.f,  15.f, 21.f
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UINumber"),
	(int)m_tGameUIInfo.eCurLevel, 0 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_SkillCool"),
		TEXT("Prototype_GameObject_UINumber"), &Desc2)))
		return E_FAIL;

	Desc2 = { 136.f, 572.f,  89.f, 78.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkillCool"),
	(int)m_tGameUIInfo.eCurLevel, 1 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_SkillCool"),
		TEXT("Prototype_GameObject_SkillCool"), &Desc2)))
		return E_FAIL;
	Desc2 = { 136.f, 569.f,  15.f, 21.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UINumber"),
	(int)m_tGameUIInfo.eCurLevel, 1 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_SkillCool"),
		TEXT("Prototype_GameObject_UINumber"), &Desc2)))
		return E_FAIL;

	Desc2 = { 71.f, 612.f,  89.f, 78.f
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkillCool"),
	(int)m_tGameUIInfo.eCurLevel, 2 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_SkillCool"),
		TEXT("Prototype_GameObject_SkillCool"), &Desc2)))
		return E_FAIL;
	Desc2 = { 71.f, 607.f,  15.f, 21.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UINumber"),
	(int)m_tGameUIInfo.eCurLevel, 2 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_SkillCool"),
		TEXT("Prototype_GameObject_UINumber"), &Desc2)))
		return E_FAIL;

	//m_pGattoIcon = static_cast<CGattoIcon*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeBack"))->back());
	return S_OK;
}

HRESULT CGameScreen::Ready_Layer_Bullet()
{
	CGameBG::IMAGE_DESC Desc1 = { 1070, 675,  22.f, 24.f
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIBullet") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_UIBullet"),
		TEXT("Prototype_GameObject_GameBG"), &Desc1)))
		return E_FAIL;

	CGattoIcon::IMAGE_DESC Desc2;
	Desc2 = { 1108.f, 675.f,  10.f, 16.f
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UINumber"),
	(int)m_tGameUIInfo.eCurLevel, 0 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_BulletNum"),
		TEXT("Prototype_GameObject_UIBullet"), &Desc2)))
		return E_FAIL;

	Desc2 = { 1095.f, 675.f,  10.f, 16.f
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UINumber"),
(int)m_tGameUIInfo.eCurLevel, 1 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_BulletNum"),
		TEXT("Prototype_GameObject_UIBullet"), &Desc2)))
		return E_FAIL;



	return S_OK;
}

HRESULT CGameScreen::Ready_Layer_Immotal()
{
	CGattoIcon::IMAGE_DESC Desc2;
	Desc2 = { 350.f, 73.f,  25.f, 31.f
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIYellowNumber"),
	(int)m_tGameUIInfo.eCurLevel, 0 };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_BulletNum"),
		TEXT("Prototype_GameObject_UIImmortal"), &Desc2)))
		return E_FAIL;
	return S_OK;
}

void CGameScreen::Control_Heart()
{
#pragma region 승준코드
	
	m_iCurHeart = m_pPlayer->Get_HP();
	if (m_iPreHeart != m_iCurHeart)
	{
		if (m_iPreHeart > m_iCurHeart) // 체력이 깎임
		{
			m_HeartList.back()->Set_HeartPop();
			m_HeartList.pop_back();

			m_pGattoIcon->Set_GattoDamaged(); // 가토이미지를 변경하고
		}

		else //체력이 참
		{
			//int iPlusHeart = m_iCurHeart - m_iPreHeart;

			////for (int i = 0; i < iPlusHeart; ++i)
			//for (int i = m_iPreHeart - 1; i < m_iCurHeart; ++i)
			//{
			//	CHeartIcon::IMAGE_DESC Desc = {};
			//	Desc = { 190.f + (50.f * (i + 1)), 72.f, 70.f, 70.f, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HeartIcon"),20, 8 };

			//	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeHeart"),
			//		TEXT("Prototype_GameObject_HeartIcon"), &Desc)))
			//		return;

			//	CHeartIcon* pHeartIcon = static_cast<CHeartIcon*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeHeart"))->back());
			//	m_HeartList.push_back(pHeartIcon);
			//}

			for (auto h : m_HeartList)
			{
				h->Set_Dead(true);
			}
			m_HeartList.clear();
			CHeartIcon::IMAGE_DESC Desc;
			for (int i = 0; i < m_iCurHeart; i++)
			{
				Desc = { 190.f + (50.f * i), 72.f, 70.f, 70.f, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HeartIcon"),20, 8 };
				//cout << "s" << endl;
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeHeart"),
					TEXT("Prototype_GameObject_HeartIcon"), &Desc)))
					return;

				CHeartIcon* pHeartIcon = static_cast<CHeartIcon*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeHeart"))->back());
				m_HeartList.push_back(pHeartIcon);
			}
		}

		m_iPreHeart = m_iCurHeart;
	}

#pragma endregion

#pragma region 새하코드

	//// HP가 0이면 더이상 감소하지 않는다
	//if (m_iPreHeart == 0)
	//	return;

	//if (m_pPlayer->Get_Damaged()) // 플레이어가 데미지를 입으면
	//{
	//	m_pGattoIcon->Set_GattoDamaged(); // 가토이미지를 변경하고

	//	auto iter = m_HeartList.back(); // 가장 마지막 하트아이콘을 변경한다
	//	iter->Set_HeartPop();
	//}

	//for (auto iter : m_HeartList)
	//{
	//	if (iter->Get_Dead())
	//		m_iCurHeart--;
	//}

	//if (m_iPreHeart != m_iCurHeart)
	//{
	//	list<CHeartIcon*> TempList;

	//	CHeartIcon::IMAGE_DESC Desc = {};

	//	for (int i = 0; i < m_iCurHeart; i++)
	//	{
	//		Desc = { 190.f + (50.f * i), 72.f, 70.f, 70.f, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HeartIcon"),20, 8 };

	//		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeHeart"),
	//			TEXT("Prototype_GameObject_HeartIcon"), &Desc)))
	//			return;

	//		CHeartIcon* pHeartIcon = static_cast<CHeartIcon*>(m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeHeart"))->back());
	//		TempList.push_back(pHeartIcon);
	//	}

	//	m_HeartList = TempList;
	//	m_iPreHeart = m_iCurHeart;
	//  --> 오브젝트 매니저에서 렌더 리스트에서 삭제해야함
	//}

#pragma endregion

	return;
}

void CGameScreen::Control_Gun()
{
	CPlayer::WEAPONS eGun = m_pPlayer->Get_Weapon();
	bool bReinforced = m_pPlayer->Get_bFever();

	if (eGun == CPlayer::RAMBO && bReinforced == false)
	{
		m_pGunSlotSelect->Set_SlotNum(CGunSlotSelect::SLOT1);
		m_pGunUsing->Set_Gun(CGunUsing::GUN1_NORMAL);
		m_pGunSlot1->Set_Gun(CGunSlot1_Rambo::ON_NORMAL);
		m_pGunSlot2->Set_Gun(CGunSlot2_Sparrow::OFF_NORMAL);
		m_pGunSlot3->Set_Gun(CGunSlot3_FlameShaker::OFF_NORMAL);

	}
	else if (eGun == CPlayer::RAMBO && bReinforced == true)
	{
		m_pGunSlotSelect->Set_SlotNum(CGunSlotSelect::SLOT1);
		m_pGunUsing->Set_Gun(CGunUsing::GUN1_FEVER);
		m_pGunSlot1->Set_Gun(CGunSlot1_Rambo::ON_FEVER);
		m_pGunSlot2->Set_Gun(CGunSlot2_Sparrow::OFF_FEVER);
		m_pGunSlot3->Set_Gun(CGunSlot3_FlameShaker::OFF_FEVER);

	}
	else if (eGun == CPlayer::SPARROW && bReinforced == false)
	{
		m_pGunSlotSelect->Set_SlotNum(CGunSlotSelect::SLOT2);
		m_pGunUsing->Set_Gun(CGunUsing::GUN2_NORMAL);
		m_pGunSlot1->Set_Gun(CGunSlot1_Rambo::OFF_NORMAL);
		m_pGunSlot2->Set_Gun(CGunSlot2_Sparrow::ON_NORMAL);
		m_pGunSlot3->Set_Gun(CGunSlot3_FlameShaker::OFF_NORMAL);

	}
	else if (eGun == CPlayer::SPARROW && bReinforced == true)
	{
		m_pGunSlotSelect->Set_SlotNum(CGunSlotSelect::SLOT2);
		m_pGunUsing->Set_Gun(CGunUsing::GUN2_FEVER);
		m_pGunSlot1->Set_Gun(CGunSlot1_Rambo::OFF_FEVER);
		m_pGunSlot2->Set_Gun(CGunSlot2_Sparrow::ON_FEVER);
		m_pGunSlot3->Set_Gun(CGunSlot3_FlameShaker::OFF_FEVER);
	}
	else if (eGun == CPlayer::FLAMESHAKER && bReinforced == false)
	{
		m_pGunSlotSelect->Set_SlotNum(CGunSlotSelect::SLOT3);
		m_pGunUsing->Set_Gun(CGunUsing::GUN3_NORMAL);
		m_pGunSlot1->Set_Gun(CGunSlot1_Rambo::OFF_NORMAL);
		m_pGunSlot2->Set_Gun(CGunSlot2_Sparrow::OFF_NORMAL);
		m_pGunSlot3->Set_Gun(CGunSlot3_FlameShaker::ON_NORMAL);
	}
	else if (eGun == CPlayer::FLAMESHAKER && bReinforced == true)
	{
		m_pGunSlotSelect->Set_SlotNum(CGunSlotSelect::SLOT3);
		m_pGunUsing->Set_Gun(CGunUsing::GUN3_FEVER);
		m_pGunSlot1->Set_Gun(CGunSlot1_Rambo::OFF_FEVER);
		m_pGunSlot2->Set_Gun(CGunSlot2_Sparrow::OFF_FEVER);
		m_pGunSlot3->Set_Gun(CGunSlot3_FlameShaker::ON_FEVER);
	}
}

CGameScreen* CGameScreen::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGameScreen* pInstance = new CGameScreen(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGameScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CGameScreen::Clone(void* pArg)
{
	CGameScreen* pInstance = new CGameScreen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CGameScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameScreen::Free()
{
	__super::Free();
}