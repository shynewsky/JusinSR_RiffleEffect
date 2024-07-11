#include "stdafx.h"
#include "..\Public\Level_SM.h"

#include "GameInstance.h"
#include "Camera_SM.h"
#include "Player_SM.h"
#include "Enemy_SM.h"

#include "Level_Loading.h"
#include "Level_Logo.h"
#include "GameScreen.h"
#include "ScreenEffect.h"
#include "TriggerBox.h"
#include <fstream>

CLevel_SM::CLevel_SM(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel { pGraphic_Device }
{
}

HRESULT CLevel_SM::Initialize()
{

	if (FAILED(Ready_Lights()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	if (FAILED(Ready_LandObjects()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera())) 
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;
	if (FAILED(Ready_Layer_MainUI()))
		return E_FAIL;
	if (FAILED(Ready_Layer_CursorUI()))
		return E_FAIL;
	
	m_pGameInstance->StopSoundALL();
	m_pGameInstance->Set_ChangedSpeed(0.3f);
	m_pGameInstance->PlayBGM(L"ElbbakDance.wav", 1.f, true);
	OpeningScript(); //오프닝 스크립트

	return S_OK;
}

void CLevel_SM::Update(_float fTimeDelta)
{

	if (m_pGameInstance->GetButtonDown(KeyType::F2))
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOGO,
			CLevel_Logo::Create(m_pGraphic_Device))))
			return;
		return;
	}
	if (m_pGameInstance->Get_ObjectList(LEVEL_SM, TEXT("Layer_Script")) != nullptr && m_bSoundTurnOn == false)
	{
		if (m_pGameInstance->Get_ObjectList(LEVEL_SM, TEXT("Layer_Script"))->empty())
		{
			m_pGameInstance->PlayBGM(L"ElbbakDance.wav", 1.f, true);
			m_bSoundTurnOn = true;
		}
	}

	//트리거 박스를 찾아서 충돌하면 화면 효과 준 후 레벨 전환
	CTriggerBox* pTriggerBox = static_cast<CTriggerBox*>(m_pGameInstance->Find_Object(LEVEL_SM, TEXT("Layer_TriggerBox")));
	if (pTriggerBox)
	{
		pTriggerBox->Get_Type();
		if (pTriggerBox->Get_EventState() && !m_bChangeLevel)
		{
			m_bChangeLevel = true;
			CScreenEffect::EFFECT_DESC Desc;
			Desc.eType = CScreenEffect::EFFECT_TYPE::ET_NOISE;
			Desc.fDuration = 2.f;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScreenEffect"), &Desc)))
				return;
		}
	}

	if (m_bChangeLevel)
		m_fLevelChangeTime += fTimeDelta;

	if (m_fLevelChangeTime > 2.f)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_LOGO))))
			MSG_BOX(TEXT("로고못감"));

		return;
	}
	m_pGameInstance->Update_Speed(fTimeDelta);
}

HRESULT CLevel_SM::Render()
{
	SetWindowText(g_hWnd, TEXT("SM레벨입니다."));
	return S_OK;
}

HRESULT CLevel_SM::Ready_Lights()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	D3DLIGHT9			LightDesc{};

	/* 빛의 종류. */
	LightDesc.Type = D3DLIGHT_DIRECTIONAL;

	/* L.Diffuse * M.Diffuse * (Shade + (L.Ambient * M.Ambient))*/

	/* 빛의 색 */
	LightDesc.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);

	/* 빛의 범위.*/

	/* 명, 암. */
	LightDesc.Direction = _float3(0.f, -1.f, 0.f);

	/* iIndex번째 광원의 정보를 장치에 저장해놓는다. */
	m_pGraphic_Device->SetLight(1, &LightDesc);

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Position = _float3(50.f, 5.f, 50.f);
	LightDesc.Range = 150.f;
	LightDesc.Attenuation1 = 0.0007f;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);

	m_pGraphic_Device->SetLight(1, &LightDesc);

	/* iIndex번째 광원을 활성화한다. */
	m_pGraphic_Device->LightEnable(0, FALSE);
	m_pGraphic_Device->LightEnable(1, TRUE);


	D3DMATERIAL9		Materials{};
	Materials.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Materials.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	m_pGraphic_Device->SetMaterial(&Materials);

	return S_OK;
}

HRESULT CLevel_SM::Ready_Layer_Camera()
{
	CCamera_SM::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 5.f, -3.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(60.0f);
	Desc.eLevel = LEVEL_SM;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_SM"), &Desc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLevel_SM::Ready_Layer_BackGround()
{
	LoadMap(L"..\\Data\\Map_SM.txt");

	return S_OK;
}

HRESULT CLevel_SM::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	Desc = {};

	Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	Desc.eLevel = LEVEL_SM;

	if (FAILED(Ready_Layer_Player(Desc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(Desc)))
		return E_FAIL;

	//스포닝풀
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Pool"), TEXT("Prototype_GameObject_Pool_SM"), nullptr)))
		return E_FAIL;

	CTriggerBox::TRIGGER_INFO trigerinfo;

	trigerinfo.eType = CTriggerBox::ET_CHANGE_LEVEL;
	trigerinfo.pos = _float3(5.f, 1.f, 5.f);

	//트리거 박스
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_TriggerBox"), TEXT("Prototype_GameObject_TriggerBox"), &trigerinfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_SM::Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player_SM"), &LandObjectDesc)))
		return E_FAIL;

	CGameObject* player = m_pGameInstance->Find_Object(LEVEL_SM, TEXT("Layer_Player"));
	
	if (!player)
		return E_FAIL;

	player->Set_Pos(_float3{ 75.f, 0.f, 18.f });

	return S_OK;
}

HRESULT CLevel_SM::Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{

	return S_OK;
}

HRESULT CLevel_SM::Ready_Layer_Effect()
{
	return S_OK;
}

HRESULT CLevel_SM::Ready_Layer_MainUI()
{
	CGameScreen::GAMEUI_DESC Desc = { LEVEL_SM };

	return S_OK;
}

HRESULT CLevel_SM::Ready_Layer_CursorUI()
{
	return S_OK;
}

HRESULT CLevel_SM::LoadMap(const wstring& _strMapPath)
{
	wifstream ifs;

	ifs.open(_strMapPath);

	//초기화
	int iObjCount;
	wstring strMapObjType;
	_float3 vScale;
	_float3 vPos;
	_float3 vAxis;
	_float fRotate;
	int iTex;

	ifs >> iObjCount;

	for (int i = 0; i < iObjCount; ++i)
	{
		ifs >> strMapObjType;
		ifs >> vScale.x >> vScale.y >> vScale.z;
		ifs >> vPos.x >> vPos.y >> vPos.z;
		ifs >> vAxis.x >> vAxis.y >> vAxis.z >> fRotate;
		ifs >> iTex;

		if (strMapObjType == L"Terrain")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"))))
				MSG_BOX(TEXT("평면 지형 생성 실패"));
		}
		else if (strMapObjType == L"Hight_Terrain")
		{
			int i = 1;
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_HeightTerrain"), &i)))
				MSG_BOX(TEXT("높이 지형 생성 실패"));
		}
		else if (strMapObjType == L"Scroll_Terrain")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScrollTerrain"))))
				MSG_BOX(TEXT("스크롤링 지형 생성 실패"));
		}
		else if (strMapObjType == L"Wall")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapWall"))))
				MSG_BOX(TEXT("벽 생성 실패"));
		}
		else if (strMapObjType == L"MapBox")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapBox"))))
				MSG_BOX(TEXT("박스 생성 실패"));
		}
		else if (strMapObjType == L"MapSphere")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapSphere"))))
				MSG_BOX(TEXT("구 생성 실패"));
		}
		else if (strMapObjType == L"MapCylinder")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapCylinder"))))
				MSG_BOX(TEXT("기둥 생성 실패"));
		}
		else if (strMapObjType == L"MapCircle")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapCircle"))))
				MSG_BOX(TEXT("원형 경기장 생성 실패"));
		}
		else
		{
			MSG_BOX(TEXT("파일 읽기 실패"));
		}

		list<CGameObject*>* pMapList = m_pGameInstance->Get_ObjectList(LEVEL_SM, TEXT("Layer_BackGround"));

		CTransform* pTransform = static_cast<CTransform*>(pMapList->back()->Find_Component(TEXT("Com_Transform")));
		CTexture* pTexture = static_cast<CTexture*>((pMapList->back())->Find_Component(TEXT("Com_Texture")));
		CCollider_Cube* pCollider = static_cast<CCollider_Cube*>((pMapList->back())->Find_Component(TEXT("Com_Collider_Cube")));

		pTransform->Scaling(vScale.x, vScale.y, vScale.z);
		pTransform->Set_State(CTransform::STATE_POSITION, vPos);
		if (fRotate != 0.f)
			pTransform->Rotation(vAxis, D3DXToRadian(fRotate));
		pTexture->Set_Frame(iTex, iTex, 0);

		if (pCollider != nullptr)
		{
			CCollider_Cube::COLLIDER_DESC CollCubeDesc;
			ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
			CollCubeDesc.fRadiusX = vScale.x * 0.5f;
			CollCubeDesc.fRadiusY = vScale.y * 0.5f;
			CollCubeDesc.fRadiusZ = vScale.z * 0.5f;
			CollCubeDesc.fOffSetX = 0.f;
			CollCubeDesc.fOffSetY = 0.f;
			CollCubeDesc.fOffsetZ = 0.f;
			pCollider->Set_CollRectDesc(CollCubeDesc);
		}
	}

	ifs.close();

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		MSG_BOX(TEXT("하늘 생성 실패"));
		return E_FAIL;
	}
	return S_OK;
}



void CLevel_SM::OpeningScript()
{

}

CLevel_SM * CLevel_SM::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_SM*		pInstance = new CLevel_SM(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_SM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_SM::Free()
{
	__super::Free();

}
