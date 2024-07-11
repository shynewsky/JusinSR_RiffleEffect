#include "stdafx.h"
#include "Pool_SM.h"
#include "GameInstance.h"


CPool_SM::CPool_SM(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CSpawningPool{ pGraphic_Device }
{
}

CPool_SM::CPool_SM(const CPool_SM& Prototype)
	:CSpawningPool{ Prototype }
{
}

HRESULT CPool_SM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPool_SM::Initialize(void* pArg)
{
	m_eCurLevel = LEVEL_SM;
	
#pragma region SM_DESC

	m_SM_Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	m_SM_Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_SM, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	m_SM_Desc.Left = true;
	m_SM_Desc.fScale = _float3{ 1.f,1.f,1.f };
	m_SM_Desc.vPos = _float3{ 50.f, 0.f, 50.f };
#pragma endregion

	m_EnemyDesc.pTerrainTransform = m_SM_Desc.pTerrainTransform;
	m_EnemyDesc.pTerrainVIBuffer = m_SM_Desc.pTerrainVIBuffer;
	m_EnemyDesc.eLevel = LEVEL_SM;


	for (int i = 0; i < 30; ++i)
	{
		if (i % 4 == 0)
			m_fRecordCooltime[i] = 1.f;
		else
			m_fRecordCooltime[i] = 1.2f;
	}

	m_fRecordCooltime[5] = 1.5f;
	m_fRecordCooltime[15] = 1.5f;
	srand(time(NULL));

	Spawn_ConveyerBelt(_float3{ 50.f, 0.f, 20.f + 16.f }, true);
	Spawn_ConveyerBelt(_float3{ 50.f, 0.f, 20.f + 20.f }, false);
	Spawn_RazorLine(_float3{ 75.f, 0.5f, 63.f }, false);
	Spawn_RazorLine(_float3{ 75.f, 0.5f, 67.f }, false);
	Spawn_RazorLine(_float3{ 75.f, 0.5f, 70.f }, false);
	Spawn_RazorLine(_float3{ 75.f, 0.5f, 73.f }, false);
	Spawn_RazorLine_Vertical(_float3{ 75.f, 0.5f, 63.f }, false);
	Spawn_RazorLine_Vertical(_float3{ 75.f, 0.5f, 73.f }, false);
	memcpy(m_fCooltime, m_fRecordCooltime, sizeof(m_fCooltime));
	return S_OK;
}

void CPool_SM::Priority_Update(_float fTimeDelta)
{
}

int CPool_SM::Update(_float fTimeDelta)
{

	SpawnLine();

	for (int i = 0; i < 4; ++i)
	{
		if (0 >= m_fOctoVerCooltime[i])
		{
			Spawn_Alien_Vertical(_float3{ 60.5f + 7.f * i, 0.f, 70.f });
			m_fOctoVerCooltime[i] = max(4.f, (rand() % 60) * 0.1f);
		}
	}

	_float fSpeed = m_pGameInstance->Get_CurSpeed();

	return OBJ_NOEVENT;
}

void CPool_SM::Late_Update(_float fTimeDelta)
{
	_float fspeed = m_pGameInstance->Get_CurSpeed();
	for (int i = 0; i < 29; ++i)
	{
		m_fCooltime[i] -= fTimeDelta * fspeed;
	}
	m_fOctoCooltime -= fTimeDelta * fspeed;
	for (int i = 0; i < 5; ++i)
	{
		m_fOctoVerCooltime[i] -= fTimeDelta * fspeed;
	}
}

HRESULT CPool_SM::Spawn_WaveShot(_float3 _vPos, _bool _bLeft)
{
	m_SM_Desc.vPos = _vPos;
	m_SM_Desc.Left = _bLeft;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Monster"),
		TEXT("Prototype_GameObject_WaveShot"), &m_SM_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPool_SM::Spawn_Dagger(_float3 _vPos, _bool _bLeft)
{
	m_SM_Desc.vPos = _vPos;
	m_SM_Desc.Left = _bLeft;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Monster"),
		TEXT("Prototype_GameObject_Dagger"), &m_SM_Desc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CPool_SM::Spawn_Slime(_float3 _vPos, _bool _bLeft)
{
	m_SM_Desc.vPos = _vPos;
	m_SM_Desc.Left = _bLeft;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Monster"),
		TEXT("Prototype_GameObject_Slime"), &m_SM_Desc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CPool_SM::Spawn_ConveyerBelt(_float3 _vPos, _bool _bLeft)
{
	m_SM_Desc.vPos = _vPos;
	m_SM_Desc.Left = _bLeft;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Monster"),
		TEXT("Prototype_GameObject_CConveyerBelt"), &m_SM_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPool_SM::Spawn_Kumiho(_float3 _vPos, _bool _bLeft)
{
	m_SM_Desc.vPos = _vPos;
	m_SM_Desc.Left = _bLeft;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Monster"),
		TEXT("Prototype_GameObject_Kumiho"), &m_SM_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPool_SM::Spawn_KumihoRunning(_float3 _vPos, _bool _bLeft)
{
	m_SM_Desc.vPos = _vPos;
	m_SM_Desc.Left = _bLeft;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Monster"),
		TEXT("Prototype_GameObject_KumihoRunning"), &m_SM_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPool_SM::Spawn_RazorLine(_float3 _vPos, _bool _bLeft)
{
	m_SM_Desc.vPos = _vPos;
	m_SM_Desc.Left = _bLeft;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Monster"),
		TEXT("Prototype_GameObject_RazorLine"), &m_SM_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPool_SM::Spawn_RazorLine_Vertical(_float3 _vPos, _bool _bLeft)
{
	m_SM_Desc.vPos = _vPos;
	m_SM_Desc.Left = _bLeft;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Monster"),
		TEXT("Prototype_GameObject_RazorLine_Vertical"), &m_SM_Desc)))
		return E_FAIL;

	return S_OK;
}

CPool_SM* CPool_SM::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPool_SM* pInstance = new CPool_SM(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPool_SM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPool_SM::Clone(void* pArg)
{
	CPool_SM* pInstance = CPool_SM::Create(m_pGraphic_Device);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone Pool_SM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPool_SM::Free()
{
	__super::Free();
}

void CPool_SM::SpawnLine()
{
	if (CheckLine(0))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 0.f}, true);
		m_fCooltime[0] = max(m_fRecordCooltime[0], (rand() % 100) / 30.f);
	}

	if (CheckLine(1))
	{
		Spawn_KumihoRunning(_float3{ 90.f, 0.f, 20.f + 1.f }, false);
		m_fCooltime[1] = max(m_fRecordCooltime[1], (rand() % 100) / 30.f);
	}

	if (CheckLine(2))
	{
		Spawn_Kumiho(_float3{ 60.f, 0.f, 20.f + 2.f }, true);
		m_fCooltime[2] = max(m_fRecordCooltime[2], (rand() % 100) / 30.f);
	}

	if (CheckLine(3))
	{
		Spawn_Kumiho(_float3{ 90.f, 0.f, 20.f + 3.f }, false);
		m_fCooltime[3] = max(m_fRecordCooltime[3], (rand() % 100) / 30.f);
	}

	if (CheckLine(4))
	{
		Spawn_Kumiho(_float3{ 60.f, 0.f, 20.f + 4.f }, true);
		m_fCooltime[4] = max(m_fRecordCooltime[4], (rand() % 100) / 30.f);
	}

	if (CheckLine(5))
	{
		Spawn_Slime(_float3{ 90.f, 0.f, 20.f + 5.f }, false);
		m_fCooltime[5] = max(m_fRecordCooltime[5], (rand() % 100) / 30.f);
	}

	if (CheckLine(6))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 6.f }, true);
		m_fCooltime[6] = max(m_fRecordCooltime[6], (rand() % 100) / 30.f);
	}

	if (CheckLine(7))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 7.f }, false);
		m_fCooltime[7] = max(m_fRecordCooltime[7], (rand() % 100) / 30.f);
	}

	if (CheckLine(8))
	{
		Spawn_Kumiho(_float3{ 60.f, 0.f, 20.f + 8.f }, true);
		m_fCooltime[8] = max(m_fRecordCooltime[8], (rand() % 100) / 30.f);
	}

	if (CheckLine(9))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 9.f}, false);
		m_fCooltime[9] = max(m_fRecordCooltime[9], (rand() % 100) / 30.f);
	}

	if (CheckLine(10))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 10.f }, true);
		m_fCooltime[10] = max(m_fRecordCooltime[10], (rand() % 100) / 30.f);
	}

	if (CheckLine(11))
	{
		Spawn_Kumiho(_float3{ 90.f, 0.f, 20.f + 11.f }, false);
		m_fCooltime[11] = max(m_fRecordCooltime[11], (rand() % 100) / 30.f);
	}

	if (CheckLine(12))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 12.f }, true);
		m_fCooltime[12] = max(m_fRecordCooltime[12], (rand() % 100) / 30.f);
	}

	if (CheckLine(13))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 13.f }, false);
		m_fCooltime[13] = max(m_fRecordCooltime[13], (rand() % 100) / 30.f);
	}

	if (CheckLine(14))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 14.f }, true);
		m_fCooltime[14] = max(m_fRecordCooltime[14], (rand() % 100) / 30.f);
	}
	//============================================ 벨트 ==============================================
	if (CheckLine(15))
	{
		Spawn_Slime(_float3{ 90.f, 0.f, 20.f + 18 }, false);
		m_fCooltime[15] = max(m_fRecordCooltime[15], (rand() % 100) / 30.f);
	}
	//============================================ 벨트 ==============================================
	if (CheckLine(16))
	{
		Spawn_Slime(_float3{ 60.f, 0.f, 20.f + 22 }, true);
		m_fCooltime[16] = max(m_fRecordCooltime[16], (rand() % 100) / 30.f);
	}

	if (CheckLine(17))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 24 }, false);
		m_fCooltime[17] = max(m_fRecordCooltime[17], (rand() % 100) / 30.f);
	}
	if (CheckLine(18))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 26 }, false);
		m_fCooltime[18] = max(m_fRecordCooltime[18], (rand() % 100) / 30.f);
	}
	if (CheckLine(19))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 28 }, true);
		m_fCooltime[19] = max(m_fRecordCooltime[19], (rand() % 100) / 30.f);
	}

	// dlc

	if (CheckLine(20))
	{
		Spawn_KumihoRunning(_float3{ 60.f, 0.f, 20.f + 29 }, true);
		m_fCooltime[20] = max(m_fRecordCooltime[20], (rand() % 100) / 30.f);
	}
	if (CheckLine(21))
	{
		Spawn_Kumiho(_float3{ 90.f, 0.f, 20.f + 30 }, false);
		m_fCooltime[21] = max(m_fRecordCooltime[21], (rand() % 100) / 30.f);
	}
	if (CheckLine(22))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 31 }, true);
		m_fCooltime[22] = max(m_fRecordCooltime[22], (rand() % 100) / 30.f);
	}
	if (CheckLine(23))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 32 }, false);
		m_fCooltime[23] = max(m_fRecordCooltime[23], (rand() % 100) / 30.f);
	}
	if (CheckLine(24))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 33 }, true);
		m_fCooltime[24] = max(m_fRecordCooltime[24], (rand() % 100) / 30.f);
	}
	if (CheckLine(25))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 34 }, false);
		m_fCooltime[25] = max(m_fRecordCooltime[25], (rand() % 100) / 30.f);
	}
	if (CheckLine(26))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 35 }, true);
		m_fCooltime[26] = max(m_fRecordCooltime[26], (rand() % 100) / 30.f);
	}
	if (CheckLine(27))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 36 }, false);
		m_fCooltime[27] = max(m_fRecordCooltime[27], (rand() % 100) / 30.f);
	}
	if (CheckLine(28))
	{
		Spawn_Dagger(_float3{ 60.f, 0.f, 20.f + 37 }, true);
		m_fCooltime[28] = max(m_fRecordCooltime[28], (rand() % 100) / 30.f);
	}
	if (CheckLine(29))
	{
		Spawn_Dagger(_float3{ 90.f, 0.f, 20.f + 38 }, false);
		m_fCooltime[29] = max(m_fRecordCooltime[29], (rand() % 100) / 30.f);
	}
}

_bool CPool_SM::CheckLine(_int _iLineNum)
{
	return 0 >= m_fCooltime[_iLineNum];
}
