#include "stdafx.h"
#include "..\Public\Level_Colosseum.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera.h"
#include "Player.h"
#include "TriggerBox.h"
#include "Client_Defines.h"
#include "Enemy.h"

#include "ScreenEffect.h"
#include <fstream>

#include "GameScreen.h"
#include "UIOrthogonal.h"
#include "UIScript.h"

#include "Fence.h"

CLevel_Colosseum::CLevel_Colosseum(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel { pGraphic_Device }
{
}

HRESULT CLevel_Colosseum::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	if (FAILED(Ready_LandObjects()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera())) // 플레이어를 타겟으로 해서, 플레이어가 만들어진 이후에 추가
		return E_FAIL;

	if (FAILED(Ready_Layer_MainUI())) // 플레이어를 타겟으로 해서, 플레이어가 만들어진 이후에 추가
		return E_FAIL;

	static_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_COLOSSEUM, TEXT("Layer_Player")))->SetUP_CameraTransform(LEVEL_COLOSSEUM);

	m_pGameInstance->StopSoundALL();
	m_pGameInstance->Load_Sound("../Sound/Stage2");
	//m_pGameInstance->PlayBGM(L"bossbattle_loop.wav", 0.5f, true);


	return S_OK;
}

void CLevel_Colosseum::Update(_float fTimeDelta)
{

	//트리거 박스를 찾아서 충돌하면 화면 효과 준 후 레벨 전환
	CTriggerBox* pTriggerBox = static_cast<CTriggerBox*>(m_pGameInstance->Find_Object(LEVEL_COLOSSEUM, TEXT("Layer_TriggerBox")));
	if (pTriggerBox)
	{
		if (pTriggerBox->Get_EventState() && !m_bChangeLevel)
		{
			m_bChangeLevel = true;
			CScreenEffect::EFFECT_DESC Desc;
			Desc.eType = CScreenEffect::EFFECT_TYPE::ET_WAVE;
			Desc.fDuration = 5.f;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScreenEffect"), &Desc)))
				return;
		}
	}

	if (m_bChangeLevel)
		m_fLevelChangeTime += fTimeDelta;

	if (m_fLevelChangeTime > 5.f)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_CHAOS))))
			MSG_BOX(TEXT("카오스못감"));

		return;

	}


}

HRESULT CLevel_Colosseum::Render()
{
	SetWindowText(g_hWnd, TEXT("콜로세움 레벨입니다."));
	return S_OK;
}

HRESULT CLevel_Colosseum::Ready_Lights()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	D3DLIGHT9			LightDesc{};

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Position = _float3(50.f, 5.f, 50.f);
	LightDesc.Range = 100.f;
	LightDesc.Attenuation1 = 0.04f;
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

HRESULT CLevel_Colosseum::Ready_Layer_Camera()
{
	CCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 5.f, -3.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(60.0f);
	Desc.eLevel = LEVEL_COLOSSEUM;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Colosseum::Ready_Layer_BackGround()
{

	LoadMap(L"..\\Data\\Map_Colosseum.txt");


	/*---------------
		펜스
	---------------*/
	CGameObject* pGameObject = nullptr;

	CFence::FENCE_DESC FenceDesc;

	//맨앞
	FenceDesc.fRadiusX = 15.f;
	FenceDesc.fRadiusY = 5.0f;
	FenceDesc.fRadiusZ = 0.1f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Fence"), TEXT("Prototype_GameObject_Fence"), &FenceDesc)))
		return E_FAIL;

	pGameObject = m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_Fence"))->back();
	pGameObject->Set_Pos(_float3(52.f, 2.5f, 25.f));
	static_cast<CTransform*>(pGameObject->Find_Component(TEXT("Com_Transform")))->Rotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(180.f));

	//맨뒤
	FenceDesc.fRadiusX = 15.f;
	FenceDesc.fRadiusY = 5.0f;
	FenceDesc.fRadiusZ = 0.1f;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Fence"), TEXT("Prototype_GameObject_Fence"), &FenceDesc)))
		return E_FAIL;

	m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_Fence"))->back()->Set_Pos(_float3(52.f, 2.5f, 74.5f));

	//좌
	FenceDesc.fRadiusX = 0.1f;
	FenceDesc.fRadiusY = 5.0f;
	FenceDesc.fRadiusZ = 15.f;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Fence"), TEXT("Prototype_GameObject_Fence"), &FenceDesc)))
		return E_FAIL;

	pGameObject = m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_Fence"))->back();
	pGameObject->Set_Pos(_float3(25.f, 2.5f, 50.f));
	static_cast<CTransform*>(pGameObject->Find_Component(TEXT("Com_Transform")))->Rotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(-90.f));

	//우
	FenceDesc.fRadiusX = 0.1f;
	FenceDesc.fRadiusY = 5.0f;
	FenceDesc.fRadiusZ = 15.f;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Fence"), TEXT("Prototype_GameObject_Fence"), &FenceDesc)))
		return E_FAIL;

	pGameObject = m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_Fence"))->back();
	pGameObject->Set_Pos(_float3(77.f, 2.5f, 51.f));
	static_cast<CTransform*>(pGameObject->Find_Component(TEXT("Com_Transform")))->Rotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(90.f));

	return S_OK;
}


HRESULT CLevel_Colosseum::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	Desc = {};

	Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	Desc.eLevel = LEVEL_COLOSSEUM;

	if (FAILED(Ready_Layer_Player(Desc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(Desc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Pet(Desc)))
		return E_FAIL;

	//랜드 커서
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_LandCursor"), TEXT("Prototype_GameObject_LandCursor"), &Desc)))
		return E_FAIL;



	CTriggerBox::TRIGGER_INFO trigerinfo;
	trigerinfo.eType = CTriggerBox::ET_CHANGE_LEVEL;
	trigerinfo.pos = _float3(50.f, 15.f, 100.f);

	//레벨 변경 트리거박스
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_TriggerBox"), TEXT("Prototype_GameObject_TriggerBox"), &trigerinfo)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Colosseum::Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &LandObjectDesc)))
		return E_FAIL;

	CGameObject* player = m_pGameInstance->Find_Object(LEVEL_COLOSSEUM, TEXT("Layer_Player"));
	if (!player)
		return E_FAIL;

	player->Set_Pos(_float3(50.f, 0.5f, 3.f));

	return S_OK;
}

HRESULT CLevel_Colosseum::Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{

	CEnemy::ENEMY_DESC Desc = {};
	Desc.eLevel = LEVEL_COLOSSEUM;
	Desc.pTerrainTransform = LandObjectDesc.pTerrainTransform;
	Desc.pTerrainVIBuffer = LandObjectDesc.pTerrainVIBuffer;
	Desc.eLevel = LEVEL_COLOSSEUM;
	Desc.fScale = { 1.f, 1.f, 1.f };
	Desc.iMaxHP = 15;
	Desc.vDir = { 50.f, 1.f, 50.f };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Monster"), TEXT("Prototype_SpawningPool_Colosseum"), &Desc)))
	{
		MSG_BOX(TEXT("콜로세움 스포닝풀 복제 실패!"));
		return E_FAIL;
	}

	//빅웜
	Desc.vDir = { 60.f, 0.f, 50.f };
	Desc.fScale = { 5.f, 5.f, 5.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_BigWorm"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Colosseum::Ready_Layer_Pet(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Pet"),
		TEXT("Prototype_GameObject_Pet"), &LandObjectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Colosseum::Ready_Layer_MainUI()
{
	CGameScreen::GAMEUI_DESC Desc = { LEVEL_COLOSSEUM };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_UI"),
		TEXT("Prototype_GameObject_GameScreen"), &Desc)))
		return E_FAIL;

	//승준이가 옴길거임^^
	{
		CUIOrthogonal::Orthogonal_DESC Desc;
		Desc.vPos = _float3(50.f, 25.f, 98.5f);
		Desc.fSizeX = 30.f;
		Desc.fSizeY = 30.f;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_UI"),
			TEXT("Prototype_GameObject_NextArrow"), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CLevel_Colosseum::LoadMap(const wstring& _strMapPath)
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
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"))))
				MSG_BOX(TEXT("평면 지형 생성 실패"));
		}
		else if (strMapObjType == L"Hight_Terrain")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_HeightTerrain"))))
				MSG_BOX(TEXT("높이 지형 생성 실패"));
		}
		else if (strMapObjType == L"Wall")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapWall"))))
				MSG_BOX(TEXT("벽 생성 실패"));
		}
		else if (strMapObjType == L"MapBox")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapBox"))))
				MSG_BOX(TEXT("박스 생성 실패"));
		}
		else if (strMapObjType == L"MapSphere")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapSphere"))))
				MSG_BOX(TEXT("구 생성 실패"));
		}
		else if (strMapObjType == L"MapCylinder")
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_MapCylinder"))))
				MSG_BOX(TEXT("기둥 생성 실패"));
		}
		else
		{
			MSG_BOX(TEXT("파일 읽기 실패"));
		}

		list<CGameObject*>* pMapList = m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"));

		CTransform* pTransform = static_cast<CTransform*>(pMapList->back()->Find_Component(TEXT("Com_Transform")));
		CTexture* pTexture = static_cast<CTexture*>((pMapList->back())->Find_Component(TEXT("Com_Texture")));
		CCollider_Cube* pCollider = static_cast<CCollider_Cube*>((pMapList->back())->Find_Component(TEXT("Com_Collider_Cube")));

		pTransform->Scaling(vScale.x, vScale.y, vScale.z);
		pTransform->Set_State(CTransform::STATE_POSITION, vPos);
		if (fRotate != 0.f)
			pTransform->Rotation(vAxis, D3DXToRadian(fRotate));
		pTexture->Set_Frame(iTex, iTex, 0);
		
		if(pCollider!=nullptr)
		{
			CCollider_Cube::COLLIDER_DESC CollCubeDesc;
			ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
			CollCubeDesc.fRadiusX = vScale.x*0.5f;
			CollCubeDesc.fRadiusY = vScale.y*0.5f;
			CollCubeDesc.fRadiusZ = vScale.z*0.5f;
			CollCubeDesc.fOffSetX = 0.f;
			CollCubeDesc.fOffSetY = 0.f;
			CollCubeDesc.fOffsetZ = 0.f;
			pCollider->Set_CollRectDesc(CollCubeDesc);
		}
	}

	ifs.close();

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		MSG_BOX(TEXT("하늘 생성 실패"));
		return E_FAIL;
	}
	return S_OK;
}



void CLevel_Colosseum::OpeningScript()
{
	{
		CUIScript::SCRIPT_DESC Desc;
		Desc.eLevel = LEVEL_GAMEPLAY;
		Desc.iTalker = 0;
		Desc.Script = TEXT("가토 : 여기서 마주치다니...\n 전투를 피할 수는 없겠군.");
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Script"),
			TEXT("Prototype_GameObject_UIScript"), &Desc);
	}
}

CLevel_Colosseum * CLevel_Colosseum::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Colosseum*		pInstance = new CLevel_Colosseum(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Colosseum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Colosseum::Free()
{
	__super::Free();

}
