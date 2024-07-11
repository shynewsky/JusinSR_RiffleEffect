#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include <iostream>

#include "MyMath.h"
#include "Camera.h"
#include "Hand.h"
#include "Head.h"
#include "Leg.h"
#include "Weapon.h"
#include "WeaponEquiped.h"
#include "Grenade.h"
#include "Gauge_Grenade.h"
#include "GrenadeRange.h"
#include "Shield.h"
#include "Aim.h"
#include "ZoomAim.h"
#include "ZoomAimShape.h"
#include "WeaponLight.h"
#include "Effect.h"

#include "GunEffect.h"
#include "ParticleSystem.h"
#include "AfterImage.h"
#include "ScreenEffect.h"

#include "../Public/Camera_Manager.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CLandObject{ Prototype }, m_fAimDistance(0.f)
{
	m_pTransformCom = { nullptr };
	m_pTransformCamera = { nullptr };
	m_pVIBufferCom = { nullptr };
	m_pColliderCom = { nullptr };

	m_eCameraMode = CAMERA_BACKHEAD;
	m_ptOldMousePos = {};
	m_pFSM = nullptr;
	memcpy(m_iBullet, m_iMAXBullet, sizeof(m_iBullet));
	m_iHP = 3;
}


HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	PlayerStartPosing();

	if (FAILED(Clone_PlayerParts()))
		return E_FAIL;

	if (FAILED(Clone_UI()))
		return E_FAIL;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	ShowCursor(false);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	Mouse_Moving(fTimeDelta);
}

int CPlayer::Update(_float fTimeDelta)
{
	m_iHP = min(m_iHP, 3);
	m_iHP = max(m_iHP, 1);
	_float3 vloooook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	int a = 0;
	//if (m_bDead)
	//	return OBJ_DEAD;
	m_pTransformCom->Move(m_vInertia);
	Friction(m_fFrictionRate);

	Key_Input(fTimeDelta);
	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_PLAYER, this);
	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	/*-------------------
	  구르기 잔상 구현부
	-------------------*/
	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_ROLL)
	{
		m_fAfterImageTime += fTimeDelta;
		if (m_fAfterImageTime > 0.04f)
		{
			m_fAfterImageTime = 0.f;
			
			CAfterImage::AFTER_DESC Desc;
			Desc.vPos = Get_Pos();
			Desc.iCurTexIndex = m_pFSM->Get_CurTextureCom()->Get_Frame().iCurrentTex;
			Desc.pTextureTag = TEXT("Prototype_Component_Texture_Player_Roll");
			Desc.iType = 2;
			Desc.eTextureLevel = LEVEL_GAMEPLAY;
			Desc.Scale = _float3(1.f, 1.f, 1.f);
			m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_AfterImage"), TEXT("Prototype_GameObject_AfterImage"), &Desc);
		}	
	}

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

	return OBJ_NOEVENT;
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	CameraAround();

	m_pFSM->Late_Update(fTimeDelta);

	m_pGameInstance->Set_Listener(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float3 vDistance = _float3(0, 0, 0);

	if (m_fHat <= 0)
	{
		if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_MONSTER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
		{
			--m_iHP;//충돌 구현부
			m_iHP = max(1, m_iHP);
			m_fHat = 3.f;
			m_fDamaged = 1.f;

			m_pGameInstance->PlayDefault(TEXT("Gatto_Hit.wav"), CHANNEL_PLAYERFX, 0.8f, false);

			CScreenEffect::EFFECT_DESC Desc;
			Desc.eType = CScreenEffect::EFFECT_TYPE::ET_HIT;
			Desc.fDuration = 1.f;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScreenEffect"), &Desc)))
				return;
		}
	}

	if (m_fHat <= 0)
	{
		if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_BULLET, this, CCollider_Manager::COLLSIION_BOX, &vDistance) && m_fHat <= 0)
		{
			--m_iHP;//충돌 구현부
			m_iHP = max(1, m_iHP);
			m_fHat = 3.f;
			m_fDamaged = 1.f;

			m_pGameInstance->PlayDefault(TEXT("Gatto_Hit.wav"), CHANNEL_PLAYERFX, 0.8f, false);

			CScreenEffect::EFFECT_DESC Desc;
			Desc.eType = CScreenEffect::EFFECT_TYPE::ET_HIT;
			Desc.fDuration = 1.f;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScreenEffect"), &Desc)))
				return;

		}
	}

	for (int i = 0; i < WEAPONS_END; ++i) //무기쿨타임 통상 감소
		m_fRemainWeaponCooltime[i] -= fTimeDelta;

	if (m_pShield != nullptr && m_fMP <= 0.1f)
	{
		m_pShield->KillSwitch();
		m_pShield = nullptr;
	}

	FrameMana(fTimeDelta);
	FrameFever(fTimeDelta);
	FrameSkillCooltime(fTimeDelta);

	m_fDamaged -= fTimeDelta;

	Add_RenderObject_Parts();

}

HRESULT CPlayer::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pFSM->Get_CurTextureCom()->Bind_Texture(m_pFSM->Get_CurTextureCom()->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Add_RenderObject_Parts()
{
	CCamera::CAMERAMODE eSrc = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Camera")))->Get_CurrentCameaMode();

	if (CAMERA_BACKHEAD == eSrc || CAMERA_REDUCING == eSrc || CAMERA_EXPENDING == eSrc)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	if ( eSrc == CAMERA_FPS)
	{
		for (int i = 0; i < PART_END; ++i)
		{
			if (nullptr != m_pParts[i])
				m_pParts[i]->Add_RenderObject(m_pParts[i]);
		}
	}


	if (CAMERA_BACKHEAD != m_eCameraMode)
		m_pUI[UI_AIM]->Add_RenderObject(m_pUI[UI_AIM]);

	if (CAMERA_ZOOM == m_eCameraMode)
	{
		m_pUI[UI_ZOOMAIM]->Add_RenderObject(m_pUI[UI_ZOOMAIM]);
		m_pUI[UI_ZOOMAIMSHAPE]->Add_RenderObject(m_pUI[UI_ZOOMAIMSHAPE]);
	}

}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_iActCooltime)
	{
		--m_iActCooltime;
		return;
	}
#pragma region 기본 이동

	if (m_pGameInstance->GetButton(KeyType::W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_pFSM->Set_Way(WAY_FRONT);
		m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_MOVE);
		m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
		m_pFSM->Set_Anitype(ANI_LOOP);
		m_vTheta = m_pTransformCamera->Get_State(CTransform::STATE_LOOK);
		m_pGameInstance->Play_NonStack(TEXT("footstep.wav"), 5, 1.f, false);
	}
	else if (m_pGameInstance->GetButton(KeyType::S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
		m_pFSM->Set_Way(WAY_BACK);
		m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_BACKMOVE);
		m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
		m_pFSM->Set_Anitype(ANI_LOOP);
		m_vTheta = -m_pTransformCamera->Get_State(CTransform::STATE_LOOK);
		m_pGameInstance->Play_NonStack(TEXT("footstep.wav"), 5, 1.f, false);
	}
	else
	{
		m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_IDLE);
		m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	}
	if (m_pGameInstance->GetButton(KeyType::A))
	{
		m_pTransformCom->Go_Left(fTimeDelta);

		if (m_pGameInstance->GetButton(KeyType::W))
		{
			if (m_eCameraMode == CAMERA_BACKHEAD)
				m_pFSM->Set_Way(WAY_FRONTL);
			m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_Anitype(ANI_LOOP);
			m_vTheta = (-m_pTransformCamera->Get_State(CTransform::STATE_RIGHT) + m_pTransformCamera->Get_State(CTransform::STATE_LOOK)) * 0.5f;
		}
		else if (m_pGameInstance->GetButton(KeyType::S))
		{
			if (m_eCameraMode == CAMERA_BACKHEAD)
				m_pFSM->Set_Way(WAY_BACKL);
			m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_BACKMOVE);
			m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_Anitype(ANI_LOOP);
			m_vTheta = (-m_pTransformCamera->Get_State(CTransform::STATE_RIGHT) - m_pTransformCamera->Get_State(CTransform::STATE_LOOK)) * 0.5f;
		}
		else
		{
			if (m_eCameraMode == CAMERA_BACKHEAD)
				m_pFSM->Set_Way(WAY_L);
			m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_Anitype(ANI_LOOP);
			m_vTheta = -m_pTransformCamera->Get_State(CTransform::STATE_RIGHT);
		}
	}
	else if (m_pGameInstance->GetButton(KeyType::D))
	{
		m_pTransformCom->Go_Right(fTimeDelta);

		if (m_pGameInstance->GetButton(KeyType::W))
		{
			if (m_eCameraMode == CAMERA_BACKHEAD)
				m_pFSM->Set_Way(WAY_FRONTR);
			m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_Anitype(ANI_LOOP);
			m_vTheta = (m_pTransformCamera->Get_State(CTransform::STATE_RIGHT) + m_pTransformCamera->Get_State(CTransform::STATE_LOOK)) * 0.5f;
		}
		else if (m_pGameInstance->GetButton(KeyType::S))
		{
			if (m_eCameraMode == CAMERA_BACKHEAD)
				m_pFSM->Set_Way(WAY_BACKR);
			m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_BACKMOVE);
			m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_Anitype(ANI_LOOP);
			m_vTheta = (m_pTransformCamera->Get_State(CTransform::STATE_RIGHT) - m_pTransformCamera->Get_State(CTransform::STATE_LOOK)) * 0.5f;
		}
		else
		{
			if (m_eCameraMode == CAMERA_BACKHEAD)
				m_pFSM->Set_Way(WAY_R);
			m_pParts[PART_BODY]->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
			m_pFSM->Set_Anitype(ANI_LOOP);
			m_vTheta = m_pTransformCamera->Get_State(CTransform::STATE_RIGHT);
		}
	}
#pragma endregion

	//구르기
	if (m_pGameInstance->GetButton(KeyType::SpaceBar) && m_fMP >= 1.f && m_fSkillCooltime[0] <= 0.f)
	{
		m_pFSM->Set_State(CFSM::OBJSTATE_ROLL);

		if (m_eCameraMode != CAMERA_BACKHEAD)
			static_cast<CCamera*>(m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Camera")))->Set_CameraMode((int)CAMERA_REDUCING);
		m_eCameraMode = CAMERA_BACKHEAD;
		m_vInertia += m_vTheta * 0.78;

		memcpy(m_iBullet, m_iMAXBullet, sizeof(m_iBullet)); //장전
		m_iActCooltime = 20;
		m_fSkillCooltime[0] = 1.f;
		m_fMP -= 1;
		m_pGameInstance->Play_NonStack(TEXT("roll.wav"), CHANNEL_PLAYERFX, 1.f, false);
		m_fHat = max(1.f, m_fHat);
	}

	//수류탄
	else if (m_pGameInstance->GetButton(KeyType::Q) && CAMERA_BACKHEAD == m_eCameraMode && m_fRemainWeaponCooltime[Grenade] && m_fMP >= 3 && m_fSkillCooltime[1] <= 0)
	{
		m_fCharge += fTimeDelta;
		if (m_eCameraMode != CAMERA_BACKHEAD)
			static_cast<CCamera*>(m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Camera")))->Set_CameraMode((int)CAMERA_REDUCING);
		m_eCameraMode = CAMERA_BACKHEAD;
		m_pGameInstance->Play_NonStack(TEXT("charge_8bit_30ms.wav"), CHANNEL_CHARGE, 0.8f, false);
		if (nullptr == m_pGauge_Grenade)
		{
			CGauge::GAUGE_DESC gaugeDesc = { g_iWinSizeX * 0.5f + 50.f, g_iWinSizeY * 0.5f - 200.f, 100.f, 10.f, 2.f, this, 0, 0, 0.1f };
			m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Gauge_Grenade"), &gaugeDesc);
			m_pGauge_Grenade = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_UI"))->back();
		}
		if (nullptr == m_pGrenadeRange)
		{
			//처음 눌렸을때 수류탄 범위 객체 생성 및 연결
			CGrenadeRange::RANGE_DESC desc;
			desc.vPos = Get_Pos();
			m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_GrenadeRange"), TEXT("Prototype_GameObject_GrenadeRange"), &desc);
			m_pGrenadeRange = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_GrenadeRange"))->back();
		}

		//누르고 있을때 범위객체도 이동시킴
		if (m_pGrenadeRange)
		{
			_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * min(m_fCharge, 2.f) * 4.f;
			m_pGrenadeRange->Set_Pos(Get_Pos() + vPos);
		}

	}

	if (m_pGameInstance->GetButtonUp(KeyType::Q) && m_fCharge != 0.1f)
	{
		CGrenade::GRENADE_DESC Desc = { m_pTransformCom->Get_State(CTransform::STATE_POSITION),
		m_pTransformCom->Get_State(CTransform::STATE_LOOK), min(m_fCharge, 2.f) * 4.f, m_pTerrainVIBuffer, m_pTerrainTransform, m_eCurLevel};
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Grenade"), &Desc)))
			MSG_BOX(TEXT("Failed To Clone Grenade"));
		m_fCharge = 0.1f;
		m_fRemainWeaponCooltime[Grenade] = m_fWeaponCooltime[Grenade];
		m_fMP -= 3.f;
		m_pGameInstance->PlayDefault(TEXT("Grenade_Shoot.wav"), CHANNEL_CHARGE, 1.f, false);
		if (nullptr != m_pGauge_Grenade)
		{
			m_pGauge_Grenade->KillSwitch();
			m_pGauge_Grenade = nullptr;
		}
		m_fSkillCooltime[1] = 2.f;
		m_pGrenadeRange->Set_Dead(true);
		m_pGrenadeRange = nullptr;
	}

	// 쉴드
	if (m_pGameInstance->GetButtonDown(KeyType::F) && nullptr == m_pShield && m_eCameraMode == CAMERA_BACKHEAD && m_fSkillCooltime[2] <= 0)
	{
		CGrenade::GRENADE_DESC Desc;
		Desc.eCurLevel = m_eCurLevel;
		Desc.pTerrainTransform = m_pTransformCom;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Shield"), &Desc)))
			MSG_BOX(TEXT("Failed to Clone Shield"));
		m_pShield = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_Player"))->back();
		m_pGameInstance->Play_NonStack(TEXT("Flash_Ready.wav"), CHANNEL_WEAPON, 1.f, false);
		m_fSkillCooltime[2] = 5.f;
	}

	//총쏘기
	if (m_pGameInstance->GetButton(KeyType::LeftMouse) && m_eCameraMode != CAMERA_BACKHEAD && m_fRemainWeaponCooltime[m_iCurWeapon] <= 0.f)
	{
		ShootingCurWeapon();
		
	}
	else if (m_pGameInstance->GetButtonUp(KeyType::LeftMouse) && FLAMESHAKER == m_iCurWeapon)
		m_pGameInstance->StopSound(CHANNEL_WEAPON);

#pragma region 무기교체

	if (m_pGameInstance->GetButtonDown(KeyType::KEY_1)) //람보
	{
		if (m_iCurWeapon != RAMBO)
		{
			static_cast<CHand*>(m_pParts[PART_HAND])->ChangeWeapon();
			static_cast<CWeapon*>(m_pParts[PART_WEAPON])->Change_Weapon(m_iCurWeapon);
			m_iCurWeapon = RAMBO;
			static_cast<WeaponEquiped*>(m_pParts[PART_EQUIPPED])->Change_EquipingWeapon(RAMBO);
		}
	}
	if (m_pGameInstance->GetButtonDown(KeyType::KEY_2)) //스페로우
	{
		if (m_iCurWeapon != SPARROW)
		{
			static_cast<CHand*>(m_pParts[PART_HAND])->ChangeWeapon();
			static_cast<CWeapon*>(m_pParts[PART_WEAPON])->Change_Weapon(m_iCurWeapon);
			m_iCurWeapon = SPARROW;
			static_cast<WeaponEquiped*>(m_pParts[PART_EQUIPPED])->Change_EquipingWeapon(SPARROW);
		}
	}
	if (m_pGameInstance->GetButtonDown(KeyType::KEY_3)) //플레임셰이커 (화방)
	{
		if (m_iCurWeapon != FLAMESHAKER)
		{
			static_cast<CHand*>(m_pParts[PART_HAND])->ChangeWeapon();
			static_cast<CWeapon*>(m_pParts[PART_WEAPON])->Change_Weapon(m_iCurWeapon);
			m_iCurWeapon = FLAMESHAKER;
			static_cast<WeaponEquiped*>(m_pParts[PART_EQUIPPED])->Change_EquipingWeapon(FLAMESHAKER);
		}
	}
#pragma endregion

#pragma region 디버깅


	/*
		//앞보기
		if (m_pGameInstance->GetButtonDown(KeyType::KEY_1))
		{
			m_pFSM->Turn_Frame(WAY_FRONT);
		}

		//뒤돌기
		if (m_pGameInstance->GetButtonDown(KeyType::KEY_2))
		{
			m_pFSM->Turn_Frame(WAY_BACK);
		}

		//키가 이전에 눌렸고 아직도 눌려있을떄
		if (m_pGameInstance->GetButton(KeyType::LeftMouse))
			cout << "mouse left clicking" << endl;
		//키가 올라왔을때
		if (m_pGameInstance->GetButtonUp(KeyType::LeftMouse))
			cout << "mouse left Up" << endl;
		//키가 처음 눌렸을때
		if (m_pGameInstance->GetButtonDown(KeyType::A))
			cout << "A Down" << endl;
	*/
#pragma endregion

	//카메라 전환
	if (m_pGameInstance->GetButtonDown(KeyType::RightMouse))
	{
		CGameObject* pCamera = m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Camera"));
		switch (m_eCameraMode)
		{
		case Client::CPlayer::CAMERA_BACKHEAD:
			if (!m_bFPSOFF)
			{
				Killswitch_Shield();
				static_cast<CCamera*>(pCamera)->Set_CameraMode((int)CAMERA_EXPENDING);
				m_eCameraMode = CAMERA_FPS;
			}
			else
			{
				m_pGameInstance->PlayDefault(TEXT("UI_Select.wav"), 6, 1.f, false);
			}
			break;
		case Client::CPlayer::CAMERA_FPS:
			if (!m_bFPSOFF && (SPARROW == m_iCurWeapon))
			{
				static_cast<CCamera*>(pCamera)->Set_CameraMode((int)CAMERA_ZOOM);
				static_cast<CZoomAim*>(m_pUI[UI_ZOOMAIM])->Reset_Frame();
				static_cast<CZoomAimShape*>(m_pUI[UI_ZOOMAIMSHAPE])->Reset_Frame();
				m_eCameraMode = CAMERA_ZOOM;
			}
			else if (SPARROW != m_iCurWeapon)
			{
				static_cast<CCamera*>(pCamera)->Set_CameraMode((int)CAMERA_REDUCING);
				m_eCameraMode = CAMERA_BACKHEAD;
				m_pGameInstance->PlayDefault(L"Reload_Analog.wav", 2, 1.f, false);
				memcpy(m_iBullet, m_iMAXBullet, sizeof(m_iBullet)); //장전
			}
			else
			{
				m_pGameInstance->PlayDefault(TEXT("UI_Select.wav"), 6, 1.f, false);
			}
			break;
		case Client::CPlayer::CAMERA_ZOOM:
			static_cast<CCamera*>(pCamera)->Set_CameraMode((int)CAMERA_REDUCING);
			m_pTransformCom->Safe_Setting_State(CTransform::STATE_LOOK, _float3{ m_vTheta.x, 0.f, m_vTheta.z });
			m_eCameraMode = CAMERA_BACKHEAD;
			m_pGameInstance->PlayDefault(L"Reload_Analog.wav", 2, 1.f, false);
			memcpy(m_iBullet, m_iMAXBullet, sizeof(m_iBullet)); //장전
			break;
		default:
			break;
		}
	}
}

void CPlayer::Friction(_float _FrictionRate)
{
	m_vInertia *= _FrictionRate;
	if (D3DXVec3Length(&m_vInertia) < 0.05)
		m_vInertia = _float3(0.f, 0.f, 0.f);
}

void CPlayer::CameraAround()
{
	if (m_eCameraMode == CAMERA_FPS)
	{
		m_pFSM->Turn_Frame(WAY_FRONT);
	}
	if (m_eCameraMode != CAMERA_BACKHEAD)
	{
		m_vTheta = m_pTransformCamera->Get_State(CTransform::STATE_LOOK);
		return;
	}
	//cos22.5 = 0.923880
	//cos67.5 = 0.382683
	_float zDot = D3DXVec3Dot(&m_vTheta, &(m_pTransformCamera->Get_State(CTransform::STATE_LOOK)));

	if (zDot >= 0.9238)
	{
		m_pFSM->Turn_Frame(WAY_FRONT);
		return;
	}
	else if (zDot < -0.9238)
	{
		m_pFSM->Turn_Frame(WAY_BACK);
		return;
	}

	_float xDot = D3DXVec3Dot(&m_vTheta, &(m_pTransformCamera->Get_State(CTransform::STATE_RIGHT)));

	if (zDot >= 0.3826)
	{
		if (xDot > 0)
		{
			m_pFSM->Turn_Frame(WAY_FRONTR);
			return;
		}

		if (xDot < 0)
		{
			m_pFSM->Turn_Frame(WAY_FRONTL);
			return;
		}
	}

	if (zDot < -0.3826)
	{
		if (xDot > 0)
		{
			m_pFSM->Turn_Frame(WAY_BACKR);
			return;
		}

		if (xDot < 0)
		{
			m_pFSM->Turn_Frame(WAY_BACKL);
			return;
		}
	}

	if (zDot < 0.3826)
	{
		if (xDot > 0)
		{
			m_pFSM->Turn_Frame(WAY_R);
			return;
		}

		if (xDot < 0)
		{
			m_pFSM->Turn_Frame(WAY_L);
			return;
		}
	}
}

void CPlayer::PlayerStartPosing()
{

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 50.f, 0.f, 50.f });
}

HRESULT CPlayer::Clone_PlayerParts()
{
	CRenderingObject::RENDERINGOBJECT_DESC Desc = { m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_LOOK) };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_RenderObject"), TEXT("Prototype_RenderObject_PlayerHand"), &Desc)))
	{
		MSG_BOX(TEXT("No Hand!"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_RenderObject"), TEXT("Prototype_RenderObject_PlayerHead"), &Desc)))
	{
		MSG_BOX(TEXT("No Head!"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_RenderObject"), TEXT("Prototype_RenderObject_PlayerBody"), &Desc)))
	{
		MSG_BOX(TEXT("No Body!"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_RenderObject"), TEXT("Prototype_RenderObject_Weapon"), &Desc)))
	{
		MSG_BOX(TEXT("No Weapon!"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_RenderObject"), TEXT("Prototype_RenderObject_WeaponEquipped"), &Desc)))
	{
		MSG_BOX(TEXT("No WeaponEquipped!"));
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_RenderObject"), TEXT("Prototype_RenderingObject_WeaponLight"), &Desc)))
	{
		MSG_BOX(TEXT("No WeaponLight!"));
	}
	//Safe_AddRef(m_pWeaponLight);

	auto iter = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_RenderObject"))->begin();
	m_pParts[0] = static_cast<CRenderingObject*>(*iter);
	m_pParts[0]->Set_TargetTransform(m_pTransformCom);
	++iter;

	m_pParts[1] = static_cast<CRenderingObject*>(*iter);
	m_pParts[1]->Set_TargetTransform(m_pTransformCom);
	++iter;

	m_pParts[2] = static_cast<CRenderingObject*>(*iter);
	m_pParts[2]->Set_TargetTransform(m_pTransformCom);
	++iter;

	m_pParts[3] = static_cast<CRenderingObject*>(*iter);
	m_pParts[3]->Set_TargetTransform(m_pTransformCom);
	++iter;

	m_pParts[4] = static_cast<CRenderingObject*>(*iter);
	m_pParts[4]->Set_TargetTransform(m_pTransformCom);
	++iter;

	m_pParts[5] = static_cast<CRenderingObject*>(*iter);
	m_pParts[5]->Set_TargetTransform(m_pTransformCom);


	return S_OK;
}

HRESULT CPlayer::Clone_UI()
{
	CAim::AIM_DESC Desc = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 50.f, 50.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_UI"), TEXT("Prototype_RenderingObject_Aim"), &Desc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_UI"), TEXT("Prototype_RenderingObject_ZoomAim"), &Desc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_UI"), TEXT("Prototype_RenderingObject_ZoomAimShape"), &Desc)))
		return E_FAIL;

	auto iter = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_UI"))->begin();
	m_pUI[UI_AIM] = static_cast<CRenderingObject*>(*iter);

	++iter;
	m_pUI[UI_ZOOMAIM] = static_cast<CRenderingObject*>(*iter);

	++iter;
	m_pUI[UI_ZOOMAIMSHAPE] = static_cast<CRenderingObject*>(*iter);

	return S_OK;
}

void CPlayer::ShootingCurWeapon()
{
	if (m_iBullet[m_iCurWeapon] > 0)
	{
		static_cast<CHand*>(m_pParts[PART_HAND])->ReloadMotion();
		m_iBullet[m_iCurWeapon] -= 1.f;
		//static_cast<CWeaponLight*>(m_pParts[PART_WEAPONLIGHT])->Shoot(m_iCurWeapon);
		if (m_eCameraMode == CAMERA_FPS)
			static_cast<WeaponEquiped*>(m_pParts[PART_EQUIPPED])->Shoot();
		m_fRemainWeaponCooltime[m_iCurWeapon] = m_bFever? m_fWeaponCooltime[m_iCurWeapon] * 0.5f : m_fWeaponCooltime[m_iCurWeapon];


		CGunEffect::GF_DESC Desc;
		Desc.vPos = _float2(550.f, 445.f);

		CParticleSystem::STATE_DESC ParticleDesc;
		ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATE_DESC));


		switch (m_iCurWeapon) //무기별 분기
		{
		case Client::CPlayer::RAMBO:
			m_pGameInstance->PlayDefault(TEXT("fire.wav"), CHANNEL_WEAPON, 0.4f, false);
			m_pGameInstance->Picking_Check_Group(CPicking_Manager::PICK_BULLET);
			Apply_Recoil(m_bFever? -5:-10);

			//효과설정
			Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_WeaponLight");

			if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_GunEffect"), TEXT("Prototype_GameObject_GunEffect"), &Desc)))
				MSG_BOX(TEXT("no GunEffect"));


			ParticleDesc.eType = CParticleSystem::PARTICLE_WEAPON;
			ParticleDesc.eTextureLevel = LEVEL_GAMEPLAY;
			ParticleDesc.strTextureTag = TEXT("Prototype_Component_Texture_Bullet");
			ParticleDesc.dDuration = 0.4f;
			ParticleDesc.dParticleLifeTime = 0.3f;
			ParticleDesc.dSpawnTime = 1;
			ParticleDesc.fParticlePerSecond = 75;
			ParticleDesc.fVelocityDeviation = 1.f;
			ParticleDesc.iMaxParticleCount = 4;
			ParticleDesc.vParticleScale = _float2(15, 15);
			ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
			ParticleDesc.iTextureNum = 0;
			ParticleDesc.vPosition = _float3(460.f, 510.f, 0.21f);
			ParticleDesc.vPosition.z += (_float)0.1;
			if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Particle"), TEXT("ProtoType_ParicleSystem"), &ParticleDesc)))
				MSG_BOX(TEXT("NO PARTICLE"));


			break;
		case Client::CPlayer::SPARROW:
			m_pGameInstance->PlayDefault(TEXT("Weapon1_Shoot.wav"), CHANNEL_WEAPON, 1.f, false);
			m_pGameInstance->Picking_Check_Group(CPicking_Manager::PICK_BULLET);
			Apply_Recoil(m_bFever ? -17 : -35);

			//효과설정
			Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_WeaponLight2");
			if (CAMERA_FPS == m_eCameraMode)
			{
				if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_GunEffect"), TEXT("Prototype_GameObject_GunEffect"), &Desc)))
					MSG_BOX(TEXT("no GunEffect"));
			}

			break;
		case Client::CPlayer::FLAMESHAKER:
		{
			//효과설정
			ParticleDesc.eType = CParticleSystem::PARTICLE_FLAME;
			ParticleDesc.eTextureLevel = LEVEL_GAMEPLAY;
			ParticleDesc.strTextureTag = TEXT("Prototype_Component_Texture_Particle");
			ParticleDesc.dDuration = 0.4f;
			ParticleDesc.dParticleLifeTime = 0.3f;
			ParticleDesc.dSpawnTime = 1;
			ParticleDesc.fParticlePerSecond = 75;
			ParticleDesc.fVelocityDeviation = 1.f;
			ParticleDesc.iMaxParticleCount = 5;
			ParticleDesc.vParticleScale = _float2(200, 200);
			ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
			ParticleDesc.iTextureNum = 1;
			ParticleDesc.vPosition = _float3(460.f, 510.f, 0.21f);
			ParticleDesc.vPosition.z += (_float)0.1;
			if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Particle"), TEXT("ProtoType_ParicleSystem"), &ParticleDesc)))
				MSG_BOX(TEXT("NO PARTICLE"));

			_float3 playerlook, deltaplayer;
			m_pGameInstance->Play_NonStack(TEXT("Weapon2_Shoot.wav"), CHANNEL_WEAPON, 1.f, false);
			auto list = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_Monster"));
			for (auto iter = list->begin(); iter != list->end(); ++iter)
			{
				deltaplayer = (*iter)->Get_Pos() - Get_Pos();
				if (D3DXVec3Length(&deltaplayer) < 6) //거리가 6 이하일 경우
				{
					D3DXVec3Normalize(&deltaplayer, &deltaplayer);
					playerlook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					D3DXVec3Normalize(&playerlook, &playerlook);
					if (D3DXVec3Dot(&playerlook, &deltaplayer) > 0.923880f) //내적이 22.5 안으로 들어올 경우 ( 전방 원뿔)
						(*iter)->KillSwitch();
				}
			}
		}
			break;
		default:
			break;
		}
	}
	else //총알 부족할때
	{
		m_pGameInstance->Play_NonStack(TEXT("Mine_Active.wav"), 7, 1.f, false);
		m_fRemainWeaponCooltime[m_iCurWeapon] = m_fWeaponCooltime[m_iCurWeapon];
	}

}

void CPlayer::FrameMana(_float _fTimeDelta)
{
	m_fHat -= _fTimeDelta;
	if (nullptr != m_pShield)
	{
		m_fMP -= _fTimeDelta * 0.5f;
	}
	else
	{
		m_fMP += _fTimeDelta; //마나 자연회복
	}
	if (m_fMP >= 6.f)
		m_fMP = 6.f;
}

void CPlayer::FrameFever(_float _fTimeDelta)
{
	if (m_fFever >= 3.f)
		m_bFever = true;

	if (m_bFever)
	{
		m_fFever -= _fTimeDelta * 0.5;
		m_fMP = 6.f;
		m_pParts[PART_WEAPON]->Set_State(CFSM::OBJSTATE_WEAPONREINFORCE);
		m_pParts[PART_EQUIPPED]->Set_State(CFSM::OBJSTATE_WEAPONREINFORCE);
		//m_fHat = 1.f;
		//m_iBullet[m_iCurWeapon] = 30;
		m_fFever = min(m_fFever, 3.f);
		memcpy(m_iBullet, m_iMAXBullet, sizeof(int) * 3);
	}

	if (m_fFever <= 0.f)
	{
		m_bFever = false;
		m_pParts[PART_WEAPON]->Set_State(CFSM::OBJSTATE_WEAPON);
		m_pParts[PART_EQUIPPED]->Set_State(CFSM::OBJSTATE_IDLE); //아오 ㅋㅋ 초기설정 잘못함
	}
}

void CPlayer::FrameSkillCooltime(_float _fTimeDelta)
{
	for (int i = 0; i < 3; ++i)
	{
		m_fSkillCooltime[i] -= _fTimeDelta;
		m_fSkillCooltime[i] = max(m_fSkillCooltime[i], 0.f);
	}
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Texture */
#pragma region 텍스처
	CTexture::TEXTUREDESC	TextureDesc{ 0 };

	TextureDesc = { 0, 0, 16, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Idle"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);

	TextureDesc = { 0, 0, 16, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Roll"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_ROLL]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_ROLL, m_pTextureCom[CFSM::OBJSTATE_ROLL]);

	TextureDesc = { 0, 0, 8, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Move"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_MOVE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_MOVE, m_pTextureCom[CFSM::OBJSTATE_MOVE]);

	TextureDesc = { 0, 0, 8, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Die"),
		TEXT("Com_Texture4"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_DIE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_DIE, m_pTextureCom[CFSM::OBJSTATE_DIE]);

#pragma endregion

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 7.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube*/
#pragma region 콜리전박스

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 0.2f;
	CollCubeDesc.fRadiusX = 0.2f;
	CollCubeDesc.fRadiusZ = 0.2f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CPlayer::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 50);

	return S_OK;
}

HRESULT CPlayer::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CPlayer::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);
	CFSM::Frame_DESC Desc;
	Desc = { 16, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);
	Desc = { 8, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_DIE, Desc);
	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_MOVE, Desc);
	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_ROLL, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);

	return S_OK;
}

void CPlayer::Mouse_Moving(_float fTimeDelta)
{
	POINT		ptMouse = {};
	GetCursorPos(&ptMouse);
	_long	MouseMove = {};

	if (MouseMove = ptMouse.x - g_iWinSizeX * 0.5)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta * MouseMove * 0.05f);
	}

	//if (MouseMove = g_iWinSizeY * 0.5 - ptMouse.y)
	//{
	//	m_fAimDistance += MouseMove;
	//}

	SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);
}

void CPlayer::Killswitch_Shield()
{
	if (nullptr != m_pShield)
	{
		static_cast<CShield*>(m_pShield)->KillSwitch();
		m_pShield = nullptr;
	}
}

void CPlayer::Apply_Recoil(_float Recoil)
{
	static_cast<CCamera*>(m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Camera")))->Set_Recoil(Recoil);
}

HRESULT CPlayer::SetUP_CameraTransform(LEVELID eLevelID)
{
	m_pTransformCamera = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(eLevelID, TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (nullptr == m_pTransformCamera)
	{
		MSG_BOX(TEXT("카메라 세팅 실패"));
		return E_FAIL;
	}
	return S_OK;
}

void CPlayer::PlusHP(_int _iAtt)
{
	m_iHP += _iAtt;
	m_iHP = min(3, m_iHP);
}

void CPlayer::HitPlayer(_int _iAtt)
{
	if (m_fHat > 0)
		return;	

	m_iHP -= _iAtt;
	m_fHat += 3.f;
	m_fDamaged = 1.f;

	m_pGameInstance->PlayDefault(TEXT("Gatto_Hit.wav"), CHANNEL_PLAYERFX, 0.8f, false);

	CScreenEffect::EFFECT_DESC Desc;
	Desc.eType = CScreenEffect::EFFECT_TYPE::ET_HIT;
	Desc.fDuration = 1.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScreenEffect"), &Desc)))
		return;
}

void CPlayer::Reset_Inertia()
{
	ZeroMemory(m_vInertia, sizeof(_float3));
}

WAY CPlayer::Get_Way()
{
	return m_pFSM->Get_Way();
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();


	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	//Safe_Release(m_pWeaponLight);

	CFSM::Destroy(m_pFSM);
}
