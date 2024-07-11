#include "stdafx.h"
#include "../Public/LogoScreen.h"

#include "GameInstance.h"
#include "LogoScreen.h"
#include "LogoBG.h"
#include "LogoBT_NewGame.h"
#include "LogoBT_Continue.h"
#include "LogoBT_BossRush.h"
#include "LogoBT_Practice.h"
#include "LogoBT_Setting.h"
#include "LogoBT_Credits.h"
#include "LogoBT_Quit.h"

CLogoScreen::CLogoScreen(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CLogoScreen::CLogoScreen(const CLogoScreen& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLogoScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogoScreen::Initialize(void* pArg)
{
	if (FAILED(Ready_Layer_LogoBG()))
		return E_FAIL;

	if (FAILED(Ready_Layer_LogoBT()))
		return E_FAIL;

	return S_OK;
}

void CLogoScreen::Priority_Update(_float fTimeDelta)
{
}

int CLogoScreen::Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CLogoScreen::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CLogoScreen::Render()
{
	return S_OK;
}


HRESULT CLogoScreen::Ready_Layer_LogoBG()
{
	CLogoBG::IMAGE_DESC Desc = { g_iWinSizeX/2, g_iWinSizeY/2,  g_iWinSizeX, g_iWinSizeY
		, LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBG"), 74, 12};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_LogoBG"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoScreen::Ready_Layer_LogoBT()
{
	CLogoBT_NewGame::BUTTON_DESC Desc1 = { 200.f, 300.f,  213.f, 40.f,  0, 1
		, LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBT")};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Button"),
		TEXT("Prototype_GameObject_LogoBT_NewGame"), &Desc1)))
		return E_FAIL;

	CLogoBT_Continue::BUTTON_DESC Desc2 = { 200.f, 360.f,  213.f, 40.f,  2, 3
		, LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBT") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Button"),
		TEXT("Prototype_GameObject_LogoBT_Continue"), &Desc2)))
		return E_FAIL;

	CLogoBT_BossRush::BUTTON_DESC Desc3 = { 200.f, 420.f,  213.f, 40.f,  4, 5
		, LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBT") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Button"),
		TEXT("Prototype_GameObject_LogoBT_BossRush"), &Desc3)))
		return E_FAIL;

	CLogoBT_Practice::BUTTON_DESC Desc4 = { 200.f, 480.f,  213.f, 40.f,  6, 7
		, LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBT") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Button"),
		TEXT("Prototype_GameObject_LogoBT_Practice"), &Desc4)))
		return E_FAIL;

	CLogoBT_Setting::BUTTON_DESC Desc5 = { 200.f, 540.f,  213.f, 40.f,  8, 9
		, LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBT") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Button"),
		TEXT("Prototype_GameObject_LogoBT_Setting"), &Desc5)))
		return E_FAIL;

	CLogoBT_Credits::BUTTON_DESC Desc6 = { 200.f, 600.f,  213.f, 40.f,  10, 11
		, LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBT") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Button"),
		TEXT("Prototype_GameObject_LogoBT_Credits"), &Desc6)))
		return E_FAIL;

	CLogoBT_Quit::BUTTON_DESC Desc7 = { 200.f, 660.f, 213.f, 40.f, 12, 13
		, LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBT") };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Button"),
		TEXT("Prototype_GameObject_LogoBT_Quit"), &Desc7)))
		return E_FAIL;

	return S_OK;
}

CLogoScreen* CLogoScreen::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoScreen* pInstance = new CLogoScreen(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CLogoScreen::Clone(void* pArg)
{
	CLogoScreen* pInstance = new CLogoScreen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoScreen::Free()
{
	__super::Free();
}