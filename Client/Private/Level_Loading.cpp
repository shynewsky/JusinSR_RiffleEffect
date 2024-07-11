#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Colosseum.h"
#include "Level_Chaos.h"
#include "Level_Palace.h"
#include "Level_Nero.h"
#include "Level_Maze.h"
#include "Level_SM.h"

#include "GameInstance.h"
#include "LogoBG.h"
#include "Terrain.h"

#include "Camera.h"
#include "Aim.h"

#pragma region 총알
#include "Bullet.h"
#include "Bullet_Arc.h"
#include "Grenade.h"
#include "GrenadeRange.h"
#include "Bullet_Sphere.h"
#include "Bullet_Cube.h"
#include "Bullet_Corn.h"
#include "Bar_Cylinder.h"
#include "Beam_Cylinder.h"
#include "Razor_Cylinder.h"
#include "Pull_Pyramid.h"
#include "Greek_Column.h"
#include "Round_Fence.h"
#include "Stone_Stand.h"
#include "WeaponLight.h"
#include "Gauge_Timer.h"
#pragma endregion

#pragma region 플레이어

#include "player.h"
#include "hand.h"
#include "head.h"
#include "leg.h"
#include "Weapon.h"
#include "WeaponEquiped.h"
#include "Shield.h"
#include "AfterImage.h"
#include "LandCursor.h"
#pragma endregion

#pragma region 보스 CHAOS
#include "Chaos.h"
#include "ChaosPatternA.h"
#include "ChaosPatternB.h"
#include "ChaosPatternC.h"
#include "Rayzer.h"
#include "AttackWay.h"
#include "ChaosBall.h"
#include "Potal.h"
#pragma endregion

#pragma region 중간보스 FELL
#include "Fell.h"
#include "FellPattern.h"
#pragma endregion

#pragma region 몬스터
#include "Monster.h"
#include "FlyingMonster.h"
#include "PracticeTarget.h"
#include "Turret.h"
#include "Red.h"
#include "Blue.h"
#include "Khaki.h"
#include "Big_worm.h"
#include "WormCorn.h"
#include "Dummy.h"
#pragma endregion

#pragma region 펫
#include "Pet.h"
#pragma endregion


#pragma region 맵
#include "Terrain.h"
#include "HeightTerrain.h"
#include "ScrollTerrain.h"
#include "Sky.h"
#include "MapBox.h"
#include "MapWall.h"
#include "MapSphere.h"
#include "MapCylinder.h"
#include "MapCircle.h"
#include "MapPyramid.h"
#include "Pool_Collosseum.h"
#include "Fence.h"
#include "Torch.h"
#include "Search_Scan.h"
#pragma endregion

#pragma region 이펙트 & 이벤트
#include "ScreenEffect.h"
#include "ParticleSystem.h"
#include "Effect.h"
#include "Effect_No_Billboard.h"
#include "GunEffect.h"
#include "TriggerBox.h"
#pragma endregion

#include "Effect.h"

#pragma region UI로고화면
#include "LogoScreen.h"
#include "LoadingBG.h"
#include "LogoBG.h"
#include "LogoBT_NewGame.h"
#include "LogoBT_Continue.h"
#include "LogoBT_BossRush.h"
#include "LogoBT_Practice.h"
#include "LogoBT_Setting.h"
#include "LogoBT_Credits.h"
#include "LogoBT_Quit.h"
#include "Mouse.h"
#pragma endregion

#pragma region MainUI
#include "GameScreen.h"
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
#include "SkillCool.h"
#include "UINumber.h"
#include "UIBullet.h"
#include "UIImmortal.h"
#pragma endregion

#pragma region BossUI
#include "BossScreen.h"
#include "BossBG.h"
#include "BossFG.h"
#include "Gauge_BossHp.h"
#include "BossFace.h"
#pragma endregion

#pragma region UI게임화면
#include "WarningSign.h"
#include "WarningSign_BarCylinder.h"
#include "WarningSign_Icedrop.h"
#include "NextArrow.h"
#include "Interection.h"
#include "IndicatorMonster.h"
#include "UIScript.h"
#include "UIPortrait.h"
#include "ZoomAim.h"
#include "ZoomAimShape.h"
#include "Gauge_Grenade.h"
#include "Gauge_Search_Scan.h"
#pragma endregion

#pragma region 보스 Nero
#include "Boss_Nero.h"
#pragma endregion

#pragma region NPC
#include "BBiz.h"
#include "Guard.h"
#include "Kane.h"
#pragma endregion

#pragma region 미니게임SM
#include "Camera_SM.h"
#include "Player_SM.h"
#include "WaveShot.h"
#include "Pool_SM.h"
#include "Dagger.h"
#include "Slime.h"
#include "Alien.h"
#include "Gauge_Chrono.h"
#include "ChronoEffect.h"
#include "ConveyerBelt.h"
#include "AlienEffect.h"
#include "Alien_Vertical.h"
#include "Kumiho.h"
#include "KumihoRunning.h"
#include "RazorLine.h"
#include "RazorLine_Vertical.h"
#pragma endregion


#include <iostream>

CLevel_Loading::CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{
}

/* unsigned (__stdcall* _beginthreadex_proc_type)(void*); */
unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLevel_Loading* pLoading = static_cast<CLevel_Loading*>(pArg);

	if (FAILED(pLoading->Loading()))
		return 1;

	return 0;
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
	m_pGameInstance->StopSoundALL();
	if (FAILED(Ready_Layer_LoadingBG()))
		return E_FAIL;

	InitializeCriticalSection(&m_CriticalSection);

	/* 추가로 생성한 스레드가 다음레벨에대한 자우너을 로드하낟. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_isFinished)
	{
		m_isFinished = false;
		//if (GetKeyState(VK_SPACE) & 0x8000)
		//{
		HRESULT			hr = {};

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			hr = m_pGameInstance->Change_Level(LEVEL_LOGO, CLevel_Logo::Create(m_pGraphic_Device));
			break;
		case LEVEL_GAMEPLAY:
			hr = m_pGameInstance->Change_Level(LEVEL_GAMEPLAY, CLevel_GamePlay::Create(m_pGraphic_Device));
			break;
		case LEVEL_COLOSSEUM:
			hr = m_pGameInstance->Change_Level(LEVEL_COLOSSEUM, CLevel_Colosseum::Create(m_pGraphic_Device));
			break;
		case LEVEL_CHAOS:
			hr = m_pGameInstance->Change_Level(LEVEL_CHAOS, CLevel_Chaos::Create(m_pGraphic_Device));
			break;
		case LEVEL_NERO:
			hr = m_pGameInstance->Change_Level(LEVEL_NERO, CLevel_Nero::Create(m_pGraphic_Device));
			break;
		case LEVEL_PALACE:
			hr = m_pGameInstance->Change_Level(LEVEL_PALACE, CLevel_Palace::Create(m_pGraphic_Device));
			break;
		case LEVEL_MAZE:
			hr = m_pGameInstance->Change_Level(LEVEL_MAZE, CLevel_Maze::Create(m_pGraphic_Device));
			break;
		case LEVEL_SM:
			hr = m_pGameInstance->Change_Level(LEVEL_SM, CLevel_SM::Create(m_pGraphic_Device));
			break;
	}
		if (FAILED(hr))
			return;
		//}
	}
}

HRESULT CLevel_Loading::Render()
{
	SetWindowText(g_hWnd, m_szLoadingText);

	return S_OK;
}

HRESULT CLevel_Loading::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_Resource_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_Resource_For_GamePlayLevel();
		break;
	case LEVEL_COLOSSEUM:
		hr = Loading_Resource_For_ColosseumLevel();
		break;
	case LEVEL_CHAOS:
		hr = Loading_Resource_For_ChaosLevel();
		break;
	case LEVEL_NERO:
		hr = Loading_Resource_For_NeroLevel();
		break;
	case LEVEL_PALACE:
		hr = Loading_Resource_For_PalaceLevel();
		break;
	case LEVEL_MAZE:
		hr = Loading_Resource_For_MazeLevel();
		break;
	case LEVEL_SM:
		hr = Loading_Resource_For_SMLevel();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_LoadingBG()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_LoadingScreen"),
		TEXT("Prototype_GameObject_LoadingBG"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

#pragma region UITexture

	/* For. Prototype_Component_Texture_LogoBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBG"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/LogoScreen/LogoBG_%d.png"), 74))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LogoBT */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBT"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/LogoScreen/TitleBT_%d.png"), 14))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Mouse"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Mouse/Mouse.png"), 1))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

#pragma region UIPrototype

	/* For. Prototype_GameObject_LogoScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoScreen"),
		CLogoScreen::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_LogoBG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoBG"),
		CLogoBG::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_LogoBT_NewGame */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoBT_NewGame"),
		CLogoBT_NewGame::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_LogoBT_Continue */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoBT_Continue"),
		CLogoBT_Continue::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_LogoBT_BossRush */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoBT_BossRush"),
		CLogoBT_BossRush::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_LogoBT_Practice */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoBT_Practice"),
		CLogoBT_Practice::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.  */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoBT_Setting"),
		CLogoBT_Setting::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_LogoBT_Credits */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoBT_Credits"),
		CLogoBT_Credits::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_LogoBT_Quit */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoBT_Quit"),
		CLogoBT_Quit::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse"),
		CMouse::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

#pragma region BackGroundTexture
	/*---------------
			MAP 텍스쳐
		---------------*/
		/* For. Prototype_Component_Texture_Terrrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Terrain/Tile_%03d.png"), 8))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURECUBE, TEXT("../Resources/Textures/Skybox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Box*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapBox"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURECUBE, TEXT("../Resources/Textures/Box/Box_%03d.dds"), 9))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Wall*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Wall"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Wall/Wall_%03d.png"), 9))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sphere*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapSphere"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Sphere/Sphere_%03d.jpg"), 6))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Cylinder*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapCylinder"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Cylinder/Cylinder_%03d.jpg"), 5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fence*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fence"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Wall/Fence.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region PlayerTexture

	/* For. Prototype_Component_Texture_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Idle/Gatto_Idle_%d.png"), 128))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Move/Gatto_Move_%d.png"), 64))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Die/Gatto_Die_%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Roll/Gatto_Roll_%d.png"), 64))))
		return E_FAIL;

	//========파츠========
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hand_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Hand/gatto_TPS_HandIdle_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hand_Change"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Hand/gatto_TPS_HandChange_%d.png"), 8))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Head_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Head/Gatto_TPS_Head_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Body_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/BodyIdle/Gatto_TPS_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Body_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/BodyMove/Gatto_TPS_Move_%d.png"), 9))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Body_BackMove"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/BodyBackMove/Gatto_TPS_BackMove_%d.png"), 8))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Weapon/Weapon_%d.png"), 12))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Reinforce"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Weapon/Weapon_Reinforce_%d.png"), 12))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponEquipped"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Weapon/WeaponEquipped_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponEquipped_Reinforce"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/Weapon/WeaponEquipped_Reinforce_%d.png"), 3))))
		return E_FAIL;

	// 쉴드
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HatField"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Player/HatField/HexAlpha_Tex.png"), 1))))
		return E_FAIL;

	// 랜드커서
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LandCursor"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/LandCursor/Cursor_%03d.png"), 8))))
		return E_FAIL;

#pragma endregion

#pragma region Pet Texture

	/* For. Prototype_Component_Texture_Pet_Bean */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pet_Bean"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Pet/Bean/Pet_Bean_%d.png"), 16))))
		return E_FAIL;

#pragma endregion

#pragma region MonsterTexture
	/* For. Prototype_Component_Texture_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Idle/Awler_Idle_%d.png"), 128))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Move/Awler_Move_%d.png"), 64))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Die/Awler_Death_%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Roll/Awler_Roll_%d.png"), 64))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Practice Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PracticeTarget"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/PracticeTarget/Target.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Flying Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlyingMonster_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/FlyingMonster/Idle/Idle_%03d.png"), 24))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlyingMonster_Attack"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/FlyingMonster/Attack/Attack_%03d.png"), 24))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlyingMonster_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/FlyingMonster/Death/Death_%03d.png"), 16))))
		return E_FAIL;

	//무지개문어 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RainbowOcto_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/rainbowocto/move/monster_rainbowocto_move_%d.png"), 27))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RainbowOcto_Die"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/rainbowocto/die/monster_rainbowocto_die_%d.png"), 51))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RainbowOcto_Attack"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/rainbowocto/skill/monster_rainbowocto_skill_%d.png"), 42))))
	//	return E_FAIL;

	//RED 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Red_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Red/Move/TerraMarineRed_Move_%d.png"), 64))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Red_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Red/Die/TerraMarineRed_die_%d.png"), 8))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Red_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Red/Roll/TerraMarineRed_Roll_%d.png"), 64))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Red_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Red/Idle/TerraMarineRed_Idle_%d.png"), 64))))
		return E_FAIL;

	/*BigWarm 텍스쳐*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BigWorm_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/big_worm/idle/big worm_idle_%d.png"), 59))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BigWorm_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/big_worm/move/big worm_move_%d.png"), 59))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BigWorm_Attack"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/big_worm/skill2/big worm_skill2_%d.png"), 59))))
		return E_FAIL;
#pragma endregion

#pragma region BulletTexture

	/* For. Prototype_Component_Texture_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/InkBullet_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RedBullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/RedBullet/InkBullet_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RedBulletEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/RedBulletEffect/InkBullet_%03d.png"), 21))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Grenade */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grenade"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/BladeSpin/BladeSpinShot_%d.png"), 4))))
		return E_FAIL;


	/* For. Prototype_Component_Texture_Grenade */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GrenadeRange"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/BladeSpin/Range.png"), 1))))
		return E_FAIL;

	/* For Prototype_Component__Texture_WeaponLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponLight"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Bullet/WeaponLight_%d.png"), 4))))
		return E_FAIL;

	/* For Prototype_Component__Texture_WeaponLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponLight2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Bullet/WeaponLight2_%d.png"), 4))))
		return E_FAIL;

#pragma endregion

#pragma region ParticleTexture

	/* For. Prototype_Component_Texture_Flare */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Particle/Particle_%03d.dds"), 3))))
		return E_FAIL;

#pragma endregion

#pragma region EffectTexture

	/* For. Prototype_Component_Texture_HitEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HitEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Hit/HitEffect_%d.png"), 5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion1/Explosion1_%d.png"), 14))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion2/Explosion2_%d.png"), 14))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion3/Explosion3_%d.png"), 16))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion4/Explosion4_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion5"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion5/Explosion5_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion6 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion6"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion6/Explosion6_%d.png"), 15))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion7 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion7"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion7/Explosion7_%d.png"), 16))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion8 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion8"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion8/Explosion8_%d.png"), 16))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion9 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion9"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion9/Explosion9_%d.png"), 16))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion10 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion10"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion10/Explosion10_%d.png"), 15))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion11 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion11"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion11/Explosion11_%d.png"), 15))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion12 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion12"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion12/Explosion12_%d.png"), 15))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion13 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion13"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Explosion13/Explosion13_%03d.png"), 12))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Chaos_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ChaosEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Chaos_Effect/ChaosEffect_%03d.png"), 12))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Electronic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Electronic"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/Electronic/Electronic_%03d.png"), 16))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GunEffect_Rifle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RifleEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/GunEffect/Rifle/GunEffect_%03d.png"), 15))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GunEffect_Sniper */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SniperEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Effect/GunEffect/Sniper/GunEffect_%03d.png"), 15))))
		return E_FAIL;

#pragma endregion

#pragma region MainUI Texture

	/* For. Prototype_Component_Texture_GameBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GameBG"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GameBG.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GameFG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GameFG"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GameFG.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GattoIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GattoIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GattoIcon_%d.png"), 9))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_HeartIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HeartIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/HeartIcon_%d.png"), 20))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GunSlotSelect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunSlotSelect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GunSlotSelect.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GunSlot1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunSlot1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GunSlot1_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GunSlot2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunSlot2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GunSlot2_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GunSlot3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunSlot3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GunSlot3_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GunUsing */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunUsing"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GunUsing_%d.png"), 6))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_SkillGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkillGauge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/SkillGauge.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_GunGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunGauge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/GunGauge.png"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region BossUI Texture

	/* For. Prototype_Component_Texture_BossBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossBG"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/BossScreen/BossBG.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossFG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossFG"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/BossScreen/BossFG.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossHp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossHp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/BossScreen/BossHp.png"), 1))))
		return E_FAIL;


#pragma endregion

#pragma region UITexture

	/* For. Prototype_Component_Texture_Aim */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Aim"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Aim/Aim_%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ZoomAim */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ZoomAim"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/ZoomUI/zoom%d.png"), 25))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ZoomAimShape */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ZoomAimShape"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/ZoomUI/zoomShape_%d.png"), 8))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_WarningSign*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WarningSign"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Utility/WarningSign.png"), 1))))
		return E_FAIL;



	/* For. Prototype_Component_Texture_Interection*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Interection"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Utility/Interection.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_NextArrow*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_NextArrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Utility/NextArrow.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_IndicatorMonster*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_IndicatorMonster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Utility/IndicatorMonster.png"), 1))))
		return E_FAIL;

	//*스크립트 텍스쳐*/
	/* For. Prototype_Component_Texture_Script*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ScriptPannel"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Script/ScriptPanel_%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Script_Portrait*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Script_PortraitRect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Script/ScriptPortraitRect.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Script_Gatto*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PortraitGatto"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Script/Gatto/GattoSmile_%d.png"), 6))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Script_Rose*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PortraitRose"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Script/Rose/RoseNormal_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Script_BBiz*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PortraitBBiz"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Script/BBiz/BBizSerious_%d.png"), 6))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Script_Kane*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PortraitKane"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Script/Kane/KaneNormal_%d.png"), 5))))
		return E_FAIL;

	// Texture_Gauge_Grenade
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Gauge_Grenade"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Gauge/Gauge_Grenade.png"), 1))))
		return E_FAIL;

	//스킬쿨
	/* For. Prototype_Component_Texture_SkillCool*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkillCool"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/SkillCool.png"), 1))))
		return E_FAIL;

	//넘버 이미지
	/* For. Prototype_Component_Texture_SkillCool*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UINumber"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/Number/%d.png"), 10))))
		return E_FAIL;

	//총알 이미지
	/* For. Prototype_Component_Texture_UIBullet*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIBullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/SymbolBullet.png"), 1))))
		return E_FAIL;

	//무적 넘버 이미지
	/* For. Prototype_Component_Texture_UIYellowNumber*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIYellowNumber"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/GameScreen/YellowNum/%d.png"), 4))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

#pragma region Model

	/* For. Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 100, 100))))
		return E_FAIL;

	/* For. Prototype_GameObject_Hight_VIBuffer_HeightTerrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Height_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, TEXT("../Resources/Textures/Terrain/Heightmap/HeightMap.bmp")))))
		return E_FAIL;

	/* For. Prototype_GameObject_Hight_VIBuffer_HeightTerrain2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Height_Terrain_Chaos"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, TEXT("../Resources/Textures/Terrain/Heightmap/HeightMapChaos.bmp")))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Cylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cylinder"),
		CVIBuffer_Cylinder::Create(m_pGraphic_Device, 0.5f, 0.5f, 1.0f, 50))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Corn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Corn"),
		CVIBuffer_Cylinder::Create(m_pGraphic_Device, 0.5f, 0.01f, 1.0f, 10))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Pyramid */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Pyramid"),
		CVIBuffer_Pyramid::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

#pragma region Map Prototype

	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_HeightTerrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HeightTerrain"),
		CHeightTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_MapBox */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapBox"),
		CMapBox::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapWall"),
		CMapWall::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_MapSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapSphere"),
		CMapSphere::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_MapCylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapCylinder"),
		CMapCylinder::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_MapCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapCircle"),
		CMapCircle::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_MapPyramid */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapPyramid"),
		CMapPyramid::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Fence */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fence"),
		CFence::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region Camera Prototype

	/* For. Prototype_GameObject_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera"),
		CCamera::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region LandObject Prototype

	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_RenderObject_PlayerHand */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderObject_PlayerHand"),
		CHand::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_RenderObject_PlayerHead */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderObject_PlayerHead"),
		CHead::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_RenderObject_PlayerBody */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderObject_PlayerBody"),
		CLeg::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_RenderObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderObject_Weapon"),
		CWeapon::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_RenderObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderObject_WeaponEquipped"),
		WeaponEquiped::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shield"),
		CShield::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Alien */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Alien"),
		CAlien::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_PracticeTarget */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PracticeTarget"),
		CPracticeTarget::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_FlyingMonster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlyingMonster"),
		CFlyingMonster::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Red */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Red"),
		CRed::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Dummy */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dummy"),
		CDummy::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_BigWorm */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BigWorm"),
		CBig_worm::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_BigWorm */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WormCorn"),
		CWormCorn::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_AfterImage */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AfterImage"),
		CAfterImage::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_LandCursor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LandCursor"),
		CLandCursor::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

#pragma region Pet Prototype

	/* For. Prototype_GameObject_Pet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pet"),
		CPet::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion


#pragma region Bullet Prototype

	/* For. Prototype_GameObject_Bullet*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
		CBullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Bullet_Arc*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Arc"),
		CBullet_Arc::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Bullet_Grenade*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grenade"),
		CGrenade::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Bullet_GrenadeRange*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GrenadeRange"),
		CGrenadeRange::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_RenderingObject_Aim*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderingObject_Aim"),
		CAim::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_RenderingObject_WeaponLight*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderingObject_WeaponLight"),
		CWeaponLight::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region Particle Prototype

	/* For. ProtoType_ParicleSystem*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("ProtoType_ParicleSystem"),
		CParticleSystem::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_ScreenEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ScreenEffect"),
		CScreenEffect::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region EffectPrototype

	/* For. Prototype_GameObject_Effect_No_Billboard*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_No_Billboard"),
		CEffect_No_Billboard::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
		CEffect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_GunEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunEffect"),
		CGunEffect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_TriggerBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TriggerBox"),
		CTriggerBox::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_RenderingObject_WarningSign */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderingObject_WarningSign"),
		CWarningSign::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region MainUI Prototype

	/* For. Prototype_GameObject_GameBG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GameBG"),
		CGameBG::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_GameFG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GameFG"),
		CGameFG::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_GattoIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GattoIcon"),
		CGattoIcon::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_HeartIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HeartIcon"),
		CHeartIcon::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_GunSlotSelect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunSlotSelect"),
		CGunSlotSelect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_GunSlot1 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunSlot1"),
		CGunSlot1_Rambo::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_GunSlot2 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunSlot2"),
		CGunSlot2_Sparrow::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_GunSlot3 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunSlot3"),
		CGunSlot3_FlameShaker::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_GunUsing */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunUsing"),
		CGunUsing::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Gauge_Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gauge_Skill"),
		CGauge_Skill::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Gauge_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gauge_Gun"),
		CGauge_Gun::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_SkillCool */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkillCool"),
		CSkillCool::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_UINumber */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UINumber"),
		CUINumber::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIBullet"),
		CUIBullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIImmortal"),
		CUIImmortal::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region BossUI Prototype

	/* For. Prototype_GameObject_BossScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossScreen"),
		CBossScreen::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_BossBG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossBG"),
		CBossBG::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_BossFG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossFG"),
		CBossFG::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Gauge_BossHp */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gauge_BossHp"),
		CGauge_BossHp::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Gauge_BossHp */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossFace"),
		CBossFace::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region UI Prototype

	/* For. Prototype_GameObject_GameScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GameScreen"),
		CGameScreen::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_ZoomAim */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderingObject_ZoomAim"),
		CZoomAim::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_ZoomAimShape */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_RenderingObject_ZoomAimShape"),
		CZoomAimShape::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_레이져, 상호작용 UI */


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rayzer"),
		CRayzer::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Interection"),
		CInterection::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NextArrow"),
		CNextArrow::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IndicatorMonster"),
		CIndicatorMonster::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIScript"),
		CUIScript::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIPortrait"),
		CUIPortrait::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gauge_Grenade"),
		CGauge_Grenade::Create(m_pGraphic_Device))))
		return E_FAIL;



#pragma endregion

	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_ColosseumLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	//BLUE 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Blue/Move/Needler_Move_%d.png"), 64))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Blue/Die/Needler_Death_%d.png"), 8))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Blue/Roll/Needler_Roll_%d.png"), 64))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Blue/Idle/Needler_Idle_%d.png"), 128))))
		return E_FAIL;

	//Gauge_Timer 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Gauge_Timer"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Gauge/TimeBar.png"), 1))))
		return E_FAIL;

	//search_Scan 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Search_Scan"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/MapObject/Search_Scan/Search_Scan_%d.png"), 4))))
		return E_FAIL;

	//Gauge_Search_Scan 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Gauge_Search_Scan"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Gauge/Gauge_Search_Scan.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	//콜로세움 웨이브
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_SpawningPool_Colosseum"),
		CPool_Collosseum::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* Prototype BLUE */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blue"),
		CBlue::Create(m_pGraphic_Device))))
		return E_FAIL;

	// Prototype Gauge_Timer
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gauge_Timer"),
		CGauge_Timer::Create(m_pGraphic_Device))))
		return E_FAIL;

	// Prototype Search_Scan
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Search_Scan"),
		CSearch_Scan::Create(m_pGraphic_Device))))
		return E_FAIL;

	// Prototype Gauge_Search_Scan
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Gameobject_Gauge_Search_Scan"),
		CGauge_Search_Scan::Create(m_pGraphic_Device))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_ChaosLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

#pragma region BOSS_Chaos 텍스쳐

	/* For. Prototype_Component_Texture_Boss_Chaos */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Spawn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/Spawn/Chaos_Spawn_%03d.png"), 90))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/Idle/Chaos_Idle_%03d.png"), 151))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Walk"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/Walk/Chaos_Walk_%03d.png"), 200))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/Move/Chaos_Move_%03d.png"), 80))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/Death/Chaos_Death_%03d.png"), 183))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_AttackA"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/AttackA/Chaos_AttackA_%03d.png"), 80))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_AttackB"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/AttackB/Chaos_AttackB_%03d.png"), 127))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_AttackC"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/AttackC/Chaos_AttackC_%03d.png"), 99))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_AttackD"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Body/AttackD/Chaos_AttackD_%03d.png"), 199))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossChaos PatternA*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternA_Spawn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternA/Spawn/Chaos_PatternA_Spawn_%03d.png"), 22))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternA_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternA/Idle/Chaos_PatternA_Idle_%03d.png"), 55))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternA_Charge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternA/Charge/Chaos_PatternA_Charge_%03d.png"), 50))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternA_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternA/Death/Chaos_PatternA_Death_%03d.png"), 49))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossChaos PatternB*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternB_Spawn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternB/Spawn/Chaos_PatternB_Spawn_%03d.png"), 39))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternB_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternB/Idle/Chaos_PatternB_Idle_%03d.png"), 63))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternB_Charge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternB/Charge/Chaos_PatternB_Charge_%03d.png"), 18))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternB_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternB/Death/Chaos_PatternB_Death_%03d.png"), 48))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossChaos PatternC*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Spawn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternC/Spawn/Chaos_PatternC_Spawn_%03d.png"), 31))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternC/Idle/Chaos_PatternC_Idle_%03d.png"), 54))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Charge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternC/Charge/Chaos_PatternC_Charge_%03d.png"), 45))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Explosion"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternC/Explosion/Chaos_PatternC_Explosion_%03d.png"), 38))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/PatternC/Death/Chaos_PatternC_Death_%03d.png"), 50))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_AttackWay*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_AttackWay"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/Utility/AttackWay.png"), 1))))
		return E_FAIL;
	/* For. Prototype_Component_Texture_ChaosBall*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_ChaosBall"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Ball/ChaosBall.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ChaosFace */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_ChaosFace"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/BossScreen/ChaosFace.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_WaterParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_WaterParticle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Particle/Water_000.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ChaosPotal */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_ChaosPotal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Chaos/Potal/Chaos_Potal.png"), 1))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ScrollTerrain"),
		CScrollTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Pool_Colosseum */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_SpqwningPool_Colosseum"),
		CPool_Collosseum::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma region BOSS_Chaos 객체 원형

	/* For. Prototype_GameObject_Texture_Boss_Chaos */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chaos"),
		CChaos::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChaosPatternA"),
		CChaosPatternA::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChaosPatternB"),
		CChaosPatternB::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChaosPatternC"),
		CChaosPatternC::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AttackWay"),
		CAttackWay::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChaosBall"),
		CChaosBall::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Potal"),
		CPotal::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion 

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_NeroLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

#pragma region Boss Nero 텍스쳐

	/* For. Prototype_Component_Texture_Nero_Appear */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Appear"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/Appear/Nero_Appear_%d.png"), 10))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_DIsappear */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_DIsappear"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/Disappear/Nero_Disappear_%d.png"), 10))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_Fly */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Fly"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/Fly/Nero_Fly_%d.png"), 64))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_FlyDown */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_FlyDown"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/FlyDown/Nero_FlyDown_%d.png"), 40))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_FlyUp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_FlyUp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/FlyUp/Nero_FlyUp_%d.png"), 40))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_Groggy */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Groggy"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/Groggy/Nero_Groggy_%d.png"), 64))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/Idle/Nero_Idle_%d.png"), 128))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/Move/Nero_Move_%d.png"), 64))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_PatternStart */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_PatternStart"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/PatternStart/Nero_PatternStart_%d.png"), 64))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_Throw */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Throw"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/Throw/Nero_Throw_%d.png"), 96))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Nero_Dead */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Dead"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Nero/Dead/Nero_Dead_%d.png"), 8))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_EarthParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_EarthParticle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Particle/Earth.png"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region Bullet 텍스쳐

	/* For. Prototype_Component_Texture_Bullet_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Bullet_Sphere"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Bullet_Sphere.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Bullet_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Bullet_Cube"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURECUBE, TEXT("../Resources/Textures/Bullet/NeroBullet/Bullet_Cube.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Bullet_Corn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Bullet_Corn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Bullet_Corn.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Bar_Cylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Bar_Cylinder"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Bar_Cylinder.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Beam_Cylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Beam_Cylinder"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/Beam_Cylinder/Beam_Cylinder_%d.png"), 100))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Razor_Cylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Razor_Cylinder"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Razor_Cylinder.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Pull_Pyramid */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Pull_Pyramid"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Pull_Pyramid.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Greek_Column */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Greek_Column"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Greek_Column.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Round_Fence */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Round_Fence"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Round_Fence.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Stone_Stand */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Stone_Stand"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Stone_Stand.png"), 1))))
		return E_FAIL;

#pragma endregion

	/* For. Prototype_Component_Texture_NeroFace*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_NeroFace"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/BossScreen/NeroFace.png"), 1))))
		return E_FAIL;

#pragma region Map 텍스쳐

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NERO, TEXT("Prototype_Component_Texture_Torch"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/MapObject/Torch/Toch_%03d.png"), 14))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

#pragma region Boss Nero 객체원형

	/* For. Prototype_GameObject_Texture_Boss_Nero */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Nero"), 
		CBoss_Nero::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Torch"),
		CTorch::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region Bullet 객체원형

	/* For. Prototype_GameObject_Bullet_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Sphere"),
		CBullet_Sphere::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Bullet_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Cube"),
		CBullet_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Bullet_Corn */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Corn"),
		CBullet_Corn::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Bar_Cylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bar_Cylinder"),
		CBar_Cylinder::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Beam_Cylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Beam_Cylinder"),
		CBeam_Cylinder::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Razor_Cylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Razor_Cylinder"),
		CRazor_Cylinder::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Pull_Pyramid */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pull_Pyramid"),
		CPull_Pyramid::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Greek_Column */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Greek_Column"),
		CGreek_Column::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Round_Fence */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Round_Fence"),
		CRound_Fence::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Stone_Stand */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stone_Stand"),
		CStone_Stand::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

	/* Prototype WarningSign_BarCylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WarningSign_BarCylinder"),
		CWarningSign_BarCylinder::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* Prototype WarningSingn_Icedrop */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WarningSign_Icedrop"),
		CWarningSign_Icedrop::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_PalaceLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_BBiz */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PALACE, TEXT("Prototype_Component_Texture_BBiz"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/NPC/BBiz/BBiz_%d.png"), 16))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PALACE, TEXT("Prototype_Component_Texture_Guard"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/NPC/Guard/Guard_%d.png"), 8))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	/* For. Prototype_GameObject_Texture_BBiz */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BBiz"),
		CBBiz::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Guard"),
		CGuard::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_MazeLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_Kane */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Kane"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/NPC/Kane/Kane_Idle_%d.png"), 16))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Turret_Spawn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Turret_Spawn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Turret/Spawn/Turret_Spawn_%03d.png"), 29))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Turret_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Turret_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Turret/Idle/Turret_Idle_%03d.png"), 61))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Turret_Attack */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Turret_Attack"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Turret/Attack/Turret_Attack_%03d.png"), 39))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Turret_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Turret_Dead"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Turret/Death/Turret_Death_%03d.png"), 30))))
		return E_FAIL;




	// Texture Khaki_Idle
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Khaki_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Khaki/Idle/TerraMarineKhaki_Idle_%d.png"), 64))))
		return E_FAIL;

	// Texture Khaki_Move
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Khaki_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Khaki/Move/TerraMarineKhaki_Move_%d.png"), 64))))
		return E_FAIL;

	// Texture Khaki_Roll
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Khaki_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Khaki/Roll/TerraMarineKhaki_Roll_%d.png"), 64))))
		return E_FAIL;

	// Texture Khaki_Die
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Khaki_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Khaki/Die/TerraMarineKhaki_die_%d.png"), 8))))
		return E_FAIL;


	/* For. Prototype_Component_Texture_Fell_Spawn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Spawn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Spawn/Fell_Spawn_%03d.png"), 46))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Idle/Fell_Idle_%03d.png"), 44))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_BurrowStart */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_BurrowStart"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Burrow_Start/Fell_BurrowStart_%03d.png"), 20))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_Burrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Burrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Burrow/Fell_Burrow_%03d.png"), 14))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_BurrowEnd*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_BurrowEnd"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Burrow_End/Fell_BurrowEnd_%03d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_DashStart*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_DashStart"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/DashStart/Fell_DashStart_%03d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_DashEnd*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_DashEnd"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/DashEnd/Fell_DashEnd_%03d.png"), 39))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_Death*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Death/Fell_Death_%03d.png"), 49))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_Pattern_Spawn*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Pattern_Spawn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Pattern/Spawn/Fell_Pattern_Spawn_%03d.png"), 29))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_Pattern_Idle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Pattern_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Pattern/Idle/Fell_Pattern_Idle%03d.png"), 40))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_Pattern_Attack*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Pattern_Attack"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Pattern/Attack/Fell_Pattern_Attack_%03d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Fell_Pattern_Death*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Pattern_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Boss/Fell/Pattern/Dead/Fell_Pattern_Death_%03d.png"), 25))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	/* For. Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAZE, TEXT("Prototype_Component_VIBuffer_Terrain_Big"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 150, 150))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	//NPC 케인
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kane"),
		CKane::Create(m_pGraphic_Device))))
		return E_FAIL;

	//터렛
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Turret"),
		CTurret::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Khaki
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Khaki"),
		CKhaki::Create(m_pGraphic_Device))))
		return E_FAIL;

	//중간보스 Fell
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fell"),
		CFell::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FellPattern"),
		CFellPattern::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_SMLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

#pragma region Kane

	/* Prototype Texture Kane_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_Kane_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Kane/Idle/Kane_Idle_%d.png"), 64))))
		return E_FAIL;

	/* Prototype Texture Kane_Roll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_Kane_Roll"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Kane/Roll/Kane_Roll_%d.png"), 32))))
		return E_FAIL;

	/* Prototype Texture Kane_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_Kane_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Kane/Move/Kane_Move_%d.png"), 32))))
		return E_FAIL;

#pragma endregion

#pragma region Enemy_SM

	/* Prototype Texture WaveShot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_WaveShot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/WaveShot/KineticWaveShot_%d.png"), 18))))
		return E_FAIL;

	/* Prototype Texture Slime */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_Slime"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Slime/Slime_%d.png"), 10))))
		return E_FAIL;

	/* Prototype Texture ChronoEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_ChronoEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/UI/ChronoBlast/ChronoBlast_%d.png"), 8))))
		return E_FAIL;

	/* Prototype Texture Conveyer belt */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_ConveyerBelt"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/MapObject/Belt/belt_%d.png"), 4))))
		return E_FAIL;

	/* Prototype Texture AlienEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_AlienEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/rainbowocto/Effect/monster_rainbowocto_die_%d.png"), 31))))
		return E_FAIL;

	/* Prototype Texture Kumiho*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_Kumiho_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Kumiho/Move/Kumiho_%d.png"), 12))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_Kumiho_Run"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Monster/Kumiho/Run/KimihoRun_%d.png"), 8))))
		return E_FAIL;

	/* Prototype Texture RazorLine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SM, TEXT("Prototype_Component_Texture_RazorLine"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Textures/Bullet/NeroBullet/Razor_Cylinder.png"), 1))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));
	m_pGameInstance->Load_Sound("StageSM");

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_Camera_SM */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_SM"),
		CCamera_SM::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype MiniPlayer_S
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_SM"),
		CPlayer_SM::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma region ENEMY_SM

	//Prototype_WaveShot
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaveShot"),
		CWaveShot::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype_Dagger
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dagger"),
		CDagger::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype_Slime
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slime"),
		CSlime::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype_ConveyerBelt
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CConveyerBelt"),
		CConveyerBelt::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype_ALien_Vertical
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ALien_Vertical"),
		CAlien_Vertical::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype_Kumiho
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kumiho"),
		CKumiho::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype_KumihoRunning
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KumihoRunning"),
		CKumihoRunning::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype_RazorLine
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RazorLine"),
		CRazorLine::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype_RazorLine_Vertical
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RazorLine_Vertical"),
		CRazorLine_Vertical::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

	//Prototype_Pool_SM
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pool_SM"),
		CPool_SM::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma region UI
	//Prototype_Gauge_Chrono
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gauge_Chrono"),
		CGauge_Chrono::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype ChronoEffect
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChronoEffect"),
		CChronoEffect::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Prototype AlienEffect
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AlienEffect"),
		CAlienEffect::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVELID eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CLevel_Loading::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	__super::Free();

}
