#include "stdafx.h"
#include "Pool_Collosseum.h"
#include "GameInstance.h"
#include "UIScript.h"
#include "Fence.h"
#include "Gauge.h"
#include "Search_Scan.h"

CPool_Collosseum::CPool_Collosseum(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CSpawningPool{pGraphic_Device}
{
}

CPool_Collosseum::CPool_Collosseum(const CPool_Collosseum& Prototype)
    :CSpawningPool{Prototype}
{
}

HRESULT CPool_Collosseum::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPool_Collosseum::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    Spawn_Blue(m_vPos);
    m_pListMonster = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_Monster"));
    
    return S_OK;
}

void CPool_Collosseum::Priority_Update(_float fTimeDelta)
{
}

int CPool_Collosseum::Update(_float fTimeDelta)
{

    switch (m_iAct)
    {
    case 1:
        if (1 == m_pListMonster->size())
            Act1();
        break;
    case 2:
        if (m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_Script"))->empty())
            Act2(); //전투
        break;
    case 3:
        if (static_cast<CSearch_Scan*>(m_pSearch_Scan)->Get_SearchValue() >= 60)
            Act3(); //탈출
		break;
    case 4:
        //if (m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_Script"))->empty())
        Act4();
    default:
        break;
    }


    return OBJ_NOEVENT;
}

void CPool_Collosseum::Late_Update(_float fTimeDelta)
{
    m_fDefaultSpawnTime -= fTimeDelta;
    m_fSpawnCoolTime -= fTimeDelta;
}

void CPool_Collosseum::Wave1()
{
    for (int i = 0; i < 4; ++i)
    {
        Spawn_Red(m_vPos + _float3{ float(i * 2), 1.f, float(i * 2) });
        Spawn_FlyingMonster(m_vPos + _float3{ float(i * 2), 1.f, float(i * 2) });
    }
    for (int j = 1; j < 4; ++j)
    {
        Spawn_Red(m_vPos + _float3{ -float(j * 2), 1.f, float(j* 2) });
    }
    m_fSpawnCoolTime = 15.f;
    m_iCurWave = 1;
    CRenderingObject::RENDERINGOBJECT_DESC rendDesc = { _float3{50.f, 1.f, 50.f}, _float3{}, 30.f, 30.f };
    m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Search_Scan"), &rendDesc);
    m_pSearch_Scan = m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_Effect"))->back();
}

void CPool_Collosseum::Wave2()
{
    Spawn_Red(_float3{ 5.f, 0.f, 45.f });
    Spawn_Red(_float3{ 3.f, 0.f, 50.f });
    Spawn_Red(_float3{ 5.f, 0.f, 55.f });
    Spawn_Blue(_float3{ 7.f, 0.f, 52.5f });
    Spawn_Blue(_float3{ 7.f, 0.f, 57.5f });
	m_fSpawnCoolTime = 15;
	m_iCurWave = 2;
}

void CPool_Collosseum::Wave3()
{
    //for (int i = 0; i < 10; ++i)
    //{
    //    Spawn_Red(_float3{ 10.f, 10.f, 50.f } + _float3{ 2 * sin(D3DXToRadian(i * 36)), 1.f, 2 * cos(D3DXToRadian(i * 36)) });
    //}
    Spawn_Red(_float3{ 97.5f, 0.f, 45.f });
    Spawn_Red(_float3{ 95.f, 0.f, 47.5f });
    Spawn_Blue(_float3{ 97, 0.f, 50.f });
    Spawn_Blue(_float3{ 93, 0.f, 50.f });
    Spawn_Red(_float3{ 95.f, 0.f, 52.5f });
    Spawn_Red(_float3{ 97.5f, 0.f, 55.f });
    /*CGauge::GAUGE_DESC gaugedesc = { g_iWinSizeX * 0.5f - 100.f, g_iWinSizeY * 0.5f + 200.f, 500.f, 10.f, 30.f, nullptr, 0, 0, 0.1f };
    m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Gauge_Timer"), &gaugedesc);*/
    m_fSpawnCoolTime = 15;
    m_iCurWave = 3;
}

void CPool_Collosseum::Wave4()
{
    //for (int i = 0; i < 3; ++i)
    //{
    //    Spawn_Big_worm(m_vPos + _float3{ 7 * sin(D3DXToRadian(i * 120)), 1.f, 7 * cos(D3DXToRadian(i * 120)) });
    //}
    //for (int i = 0; i < 3; ++i)
    //{
    //    Spawn_Red(_float3{ 45.f + i * 5.f, 0.f, 95.f });
    //}
    //for (int i = 0; i < 4; ++i)
    //{
    //    Spawn_Red(_float3{47.5f + 2.5f * i, 0.f, 97.5f});
    //}
    //m_fSpawnCoolTime = 15;
    m_iCurWave = 4;
}

void CPool_Collosseum::Wave5()
{
    for (int i = 0; i < 3; ++i)
    {
        Spawn_FlyingMonster(_float3{ 45.f + 5.f * i, 0.f, 2.5f });
        Spawn_Red(_float3{ 45.f + 5.f * i, 0.f, 5.f });
        Spawn_FlyingMonster(_float3{ 45.f + 5.f * i, 0.f, 7.5f });
    }
    Spawn_Blue(_float3{ 47.5f, 0.f, 7.5f });
    Spawn_Blue(_float3{ 52.5f, 0.f, 7.5f });
    m_fSpawnCoolTime = 15;
    m_iCurWave = 5;
}

void CPool_Collosseum::DefaultSpawn()
{

	Spawn_Red(_float3{ 50.f, 0.f, 0.f });
	Spawn_Red(_float3{ 0.f, 0.f, 50.f });
	Spawn_Red(_float3{ 100.f, 0.f, 50.f });
	Spawn_Red(_float3{ 50.f, 0.f, 100.f });

    m_fDefaultSpawnTime = 13;
}

void CPool_Collosseum::Act1()
{
	CUIScript::SCRIPT_DESC Desc;
	Desc.eLevel = m_eCurLevel;
	Desc.iTalker = 0;
	Desc.Script = TEXT("가토 : 생각보다 조용한걸?");
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Script"),
        TEXT("Prototype_GameObject_UIScript"), &Desc)))
    {
        MSG_BOX(TEXT("첫몹 트리거 스크립트 생성 실패!"));
    }
    Desc.iTalker = 1;
    Desc.Script = TEXT("로즈 : 적들이 접근하고있어! \n길을 찾을 테니 조금만 버텨!");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Script"),
		TEXT("Prototype_GameObject_UIScript"), &Desc)))
	{
		MSG_BOX(TEXT("첫몹 트리거 스크립트 생성 실패!"));
	}
    m_bStartStage = true; // 웨이브 시작
    m_iAct = 2;
    m_fSpawnCoolTime = 0.f;


}

void CPool_Collosseum::Act2()
{
    if (!m_bBgmOn)
    {
        m_pGameInstance->PlayBGM(TEXT("DarkRoom.wav"), 0.8f, true);
        m_bBgmOn = true;
        for (auto Fence : *m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_Fence")))
        {
            static_cast<CFence*>(Fence)->Set_Defense(true);
        }
        
    }
    if (m_fSpawnCoolTime <= 0)
    {
        switch (m_iCurWave)
        {
        case 0:
            Wave1();
            break;
        case 1:
            Wave2();
            break;
        case 2:
            Wave3();
            break;
        case 3:
            Wave4();
            break;
        case 4:
            Wave5();
            break;
        case 5:
            m_iAct = 3;
            break;
        default:
            break;
        }
    }

    if (m_bStartStage && m_fDefaultSpawnTime <= 0)
        DefaultSpawn();
}

void CPool_Collosseum::Act3()
{
    CUIScript::SCRIPT_DESC Desc;
    Desc.eLevel = m_eCurLevel;
    Desc.iTalker = 1;
    Desc.Script = TEXT("로즈 : 방화벽을 해제했어!");
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Script"),
        TEXT("Prototype_GameObject_UIScript"), &Desc)))
    {
        MSG_BOX(TEXT("액트3 스크립트 생성 실패!"));
    }
    Desc.iTalker = 0;
    Desc.Script = TEXT("가토 : 여길 벗어나자!");
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Script"),
        TEXT("Prototype_GameObject_UIScript"), &Desc)))
    {
        MSG_BOX(TEXT("액트3 스크립트 생성 실패!"));
    }

    m_iAct = 4;



    //도망
}

void CPool_Collosseum::Act4()
{
    m_pGameInstance->Clear_ObjectList(m_eCurLevel, TEXT("Layer_Fence"));
    m_iAct = 5;
}

CPool_Collosseum* CPool_Collosseum::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CPool_Collosseum* pInstance = new CPool_Collosseum(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPool_Colloseum"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPool_Collosseum::Clone(void* pArg)
{
    CPool_Collosseum* pInstance = CPool_Collosseum::Create(m_pGraphic_Device);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone Pool_Colosseum"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPool_Collosseum::Free()
{
    __super::Free();
}
