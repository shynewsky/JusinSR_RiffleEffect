#include "stdafx.h"
#include "../Public/BossScreen.h"

#include "GameInstance.h"

#include "BossBG.h"
#include "BossFG.h"
#include "Gauge_BossHp.h"
#include "Enemy.h"

CBossScreen::CBossScreen(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CBossScreen::CBossScreen(const CBossScreen& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CBossScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossScreen::Initialize(void* pArg)
{
	BOSSUI_DESC* pDesc = static_cast<BOSSUI_DESC*>(pArg);
	m_tGameUIInfo.eCurLevel = pDesc->eCurLevel;

	if (FAILED(Ready_Layer_BossBG()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Gauge()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Frame()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Face()))
		return E_FAIL;

	Target = m_pGameInstance->Find_Object(m_tGameUIInfo.eCurLevel, TEXT("Layer_Monster"));

	return S_OK;
}

void CBossScreen::Priority_Update(_float fTimeDelta)
{

}

int CBossScreen::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CBossScreen::Late_Update(_float fTimeDelta)
{
	if (static_cast<CEnemy*>(Target)->Get_Hp()<=0
		|| Target == nullptr || Target->Get_Dead() == true)
	{
		pBossBG->Set_Dead(true);
		pBossGauge->Set_Dead(true);
		pFace->Set_Dead(true);
		pBossFrame->Set_Dead(true);

		m_bDead = true;
	}
}

HRESULT CBossScreen::Render()
{
	return S_OK;
}

HRESULT CBossScreen::Ready_Layer_BossBG()
{
	CBossBG::IMAGE_DESC Desc1 = { g_iWinSizeX/2, g_iWinSizeY/2,  g_iWinSizeX, g_iWinSizeY
		, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossBG")};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeBack"),
		TEXT("Prototype_GameObject_BossBG"), &Desc1)))
		return E_FAIL;

	pBossBG = m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeBack"))->back();
	if (!pBossBG)
		return E_FAIL;

	return S_OK;
}

HRESULT CBossScreen::Ready_Layer_Gauge()
{
	CGauge_BossHp::GAUGE_DESC Desc1 =
	{
		//g_iWinSizeX / 2, 82.f	// 위치
		820.f, 638.f	// 위치
		, 361.f, 10.f	// 크기
		, 200.f			// MaxValue
		, m_pGameInstance->Find_Object(m_tGameUIInfo.eCurLevel, TEXT("Layer_Monster"))
		, 0, 0, 0.f
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeCenter"),
		TEXT("Prototype_GameObject_Gauge_BossHp"), &Desc1)))
		return E_FAIL;

	pBossGauge = m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeCenter"))->back();
	if (!pBossGauge)
		return E_FAIL;

	return S_OK;
}

HRESULT CBossScreen::Ready_Layer_Frame()
{
	CBossFG::IMAGE_DESC Desc1 = { g_iWinSizeX / 2, g_iWinSizeY / 2,  g_iWinSizeX, g_iWinSizeY
	, LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossFG") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeFront"),
		TEXT("Prototype_GameObject_BossFG"), &Desc1)))
		return E_FAIL;

	pBossFrame = m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_GaugeFront"))->back();
	if (!pBossFrame)
		return E_FAIL;

	return S_OK;
}

HRESULT CBossScreen::Ready_Layer_Face()
{
	CBossFG::IMAGE_DESC Desc1;

	if (m_tGameUIInfo.eCurLevel == LEVEL_NERO)
	{
		Desc1 = { g_iWinSizeX / 2, 38.f, 50, 50
	, LEVEL_NERO, TEXT("Prototype_Component_Texture_NeroFace") };

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_BossFace"),
			TEXT("Prototype_GameObject_BossFG"), &Desc1)))
			return E_FAIL;
	}
	else if (m_tGameUIInfo.eCurLevel == LEVEL_CHAOS)
	{
		Desc1 = { g_iWinSizeX / 2, 38.f, 50, 50
	, LEVEL_CHAOS, TEXT("Prototype_Component_Texture_ChaosFace") };

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGameUIInfo.eCurLevel, TEXT("Layer_BossFace"),
			TEXT("Prototype_GameObject_BossFace"), &Desc1)))
			return E_FAIL;
	}
	pFace = m_pGameInstance->Get_ObjectList(m_tGameUIInfo.eCurLevel, TEXT("Layer_BossFace"))->back();
	if (!pFace)
		return E_FAIL;



	return S_OK;
}

CBossScreen* CBossScreen::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossScreen* pInstance = new CBossScreen(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBossScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBossScreen::Clone(void* pArg)
{
	CBossScreen* pInstance = new CBossScreen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBossScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossScreen::Free()
{
	__super::Free();
}