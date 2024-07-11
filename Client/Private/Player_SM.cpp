#include "stdafx.h"
#include "Player_SM.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Gauge_Chrono.h"
#include "FSM.h"
#include "ScreenEffect.h"

CPlayer_SM::CPlayer_SM(LPDIRECT3DDEVICE9 pGrapihc_Device)
	:CLandObject{ pGrapihc_Device }
{
}

CPlayer_SM::CPlayer_SM(const CPlayer_SM& Prototype)
	:CLandObject{ Prototype }
{
}

HRESULT CPlayer_SM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_SM::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SetUp_FSM();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	
	return S_OK;
}

void CPlayer_SM::Priority_Update(_float fTimeDelta)
{
}


int CPlayer_SM::Update(_float fTimeDelta)
{
	Key_Input(fTimeDelta);
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_PLAYER, this);

	return OBJ_NOEVENT;
}

void CPlayer_SM::Late_Update(_float fTimeDelta)
{
	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_BULLET, this, CCollider_Manager::COLLSIION_BOX, &vDistance)
		|| Get_Pos().x <= 60 || Get_Pos().x >= 90)
	{
		m_pTransformCom->GOTO(_float3{ 75.f, 0.f, 18.f });
	}
	m_pFSM->Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_fActCooltime -= fTimeDelta;

	if (Get_Pos().z > 84)
	{
		SetUp_OnTerrain(m_pTransformCom, 0.7f);
	}
	else
	{
		SetUp_OnTerrain(m_pTransformCom, 0.5f);
	}
}

HRESULT CPlayer_SM::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pFSM->Get_CurTextureCom()->Bind_Texture(m_pFSM->Get_CurTextureCom()->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	_float4x4		ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (Reset_RenderState())
		return E_FAIL;

	return S_OK;

}

void CPlayer_SM::Move_Player(_float3 _vDir)
{
	m_pTransformCom->Move(_vDir);
}

HRESULT CPlayer_SM::Ready_Component()
{

	/* For.Com_Texture */
#pragma region 텍스처
	CTexture::TEXTUREDESC	TextureDesc{ 0 };

	TextureDesc = { 0, 0, 64, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_SM, TEXT("Prototype_Component_Texture_Kane_Idle"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);

	TextureDesc = { 0, 0, 32, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_SM, TEXT("Prototype_Component_Texture_Kane_Roll"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_ROLL]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_ROLL, m_pTextureCom[CFSM::OBJSTATE_ROLL]);

	TextureDesc = { 0, 0, 32, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_SM, TEXT("Prototype_Component_Texture_Kane_Move"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_MOVE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_MOVE, m_pTextureCom[CFSM::OBJSTATE_MOVE]);
#pragma endregion
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

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

HRESULT CPlayer_SM::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 249);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, FALSE);

	return S_OK;
}

HRESULT CPlayer_SM::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, TRUE);

	return S_OK;
}

HRESULT CPlayer_SM::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);
	CFSM::Frame_DESC desc;
	desc = { 16,4 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, desc);

	desc = { 8,4 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_ROLL, desc);

	desc = { 8,4 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_MOVE, desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);

	return S_OK;
}

void CPlayer_SM::Key_Input(_float _fTimedelta)
{
#pragma region 기본이동
	if (m_pGameInstance->GetButton(KeyType::W))
	{
		m_pFSM->Set_Way(WAY(2));
		m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
		m_pTransformCom->Move(_float3{ 0.f, 0.f, 8.f * _fTimedelta });
	}
	else if (m_pGameInstance->GetButtonUp(KeyType::W))
		m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);

	if (m_pGameInstance->GetButton(KeyType::D))
	{
		m_pFSM->Set_Way(WAY(1));
		m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
		m_pTransformCom->Move(_float3{ 8.f * _fTimedelta, 0.f, 0.f });
	}
	else if (m_pGameInstance->GetButtonUp(KeyType::D))
		m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);

	if (m_pGameInstance->GetButton(KeyType::A))
	{
		m_pFSM->Set_Way(WAY(3));
		m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
		m_pTransformCom->Move(_float3{ - 8.f * _fTimedelta, 0.f, 0.f });
	}
	else if (m_pGameInstance->GetButtonUp(KeyType::A))
		m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);

	if (m_pGameInstance->GetButton(KeyType::S))
	{
		m_pFSM->Set_Way(WAY(0));
		m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
		m_pTransformCom->Move(_float3{ 0.f, 0.f, - 8.f * _fTimedelta });
	}
	else if (m_pGameInstance->GetButtonUp(KeyType::S))
		m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
#pragma endregion

	if (m_pGameInstance->GetButton(KeyType::SpaceBar) && m_fActCooltime <= 0)
	{
		CGauge::GAUGE_DESC desc = { 150 + (float)g_iWinSizeX * 0.5, (float)g_iWinSizeY - 100.f, 300.f, 10.f, 3.f, nullptr, 0, 0, 0.f };
		m_pGameInstance->Change_Speed(3.f);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_UI"),
			TEXT("Prototype_GameObject_Gauge_Chrono"), &desc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_UI"),
			TEXT("Prototype_GameObject_ChronoEffect"), nullptr);
		m_fActCooltime = 3.f;

		CScreenEffect::EFFECT_DESC Desc;
		Desc.eType = CScreenEffect::EFFECT_TYPE::ET_TIME;
		Desc.fDuration = 12.f;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScreenEffect"), &Desc)))
			return;
	}
}

CPlayer_SM* CPlayer_SM::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer_SM* pInstance = new CPlayer_SM(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CPlayer_SM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_SM::Clone(void* pArg)
{
	CPlayer_SM* pInstance = new CPlayer_SM(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : Cplayer_SM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_SM::Free()
{
	__super::Free();
	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
		Safe_Release(m_pTextureCom[i]);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	CFSM::Destroy(m_pFSM);
}
