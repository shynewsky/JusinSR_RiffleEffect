#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "Level_Loading.h"
#include "GameInstance.h"

#include "LogoBT_NewGame.h"

CLevel_Logo::CLevel_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel { pGraphic_Device }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_LogoScreen()))
		return E_FAIL;

	m_pGameInstance->StopSoundALL();
	m_pGameInstance->Load_Sound("StageLogo");
	m_pGameInstance->PlayBGM(L"BGM_Logo.wav", 0.5f, true);
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	Check_ButtonClick();



}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Lights()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_LogoScreen()
{

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_LogoScreen"),
		TEXT("Prototype_GameObject_Mouse"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_LogoScreen"),
		TEXT("Prototype_GameObject_LogoScreen"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Check_ButtonClick()
{
	/* 버튼 리스트 가져오기 */
	auto ButtonList = m_pGameInstance->Get_ObjectList(LEVEL_LOGO, TEXT("Layer_Button"));

	/* 버튼을 순회한다 */
	auto iter = ButtonList->begin();

	/* 1번버튼(새로하기) 가리키기 */
	for (size_t i = 0; i < 0; i++)
		++iter;

	CButton* pButton = dynamic_cast<CButton*>(*iter);

	bool bMouseOver = pButton->Get_MouseOver();

	if (bMouseOver == true
		&& m_pGameInstance->GetButton(KeyType::LeftMouse))
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY))))
			return;
	}
}

CLevel_Logo * CLevel_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}
