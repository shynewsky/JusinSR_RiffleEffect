#include "stdafx.h"
#include "Big_worm.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Player.h"
#include "WarningSign.h"
#include "Effect.h"


CBig_worm::CBig_worm(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CEnemy{pGraphic_Device}
{
}

CBig_worm::CBig_worm(const CBig_worm& Prototype)
	:CEnemy(Prototype)
{
}

HRESULT CBig_worm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBig_worm::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;


	ENEMY_DESC Desc = *(ENEMY_DESC*)pArg;
	m_pTransformCom->Scaling(Desc.fScale.x, Desc.fScale.y, Desc.fScale.z);
	m_pTransformCom->GOTO(Desc.vDir);
	m_iHP = m_iMaxHP = Desc.iMaxHP;
	m_fAttackRange = 10.f;
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	return S_OK;
}

void CBig_worm::Priority_Update(_float fTimeDelta)
{
	if (!m_bDead)
		m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
	Compute_DeltaPlayer();
	AGGROPlayer();
	m_fAttackCooltime -= fTimeDelta;
}

int CBig_worm::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
		CEffect::EFFECT_DESC effDesc;
		effDesc.iFrameSpeed = 0.3f;
		effDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion7");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &effDesc)))
			MSG_BOX(TEXT("NO EFFECT"));

		m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(Get_Pos());
		return OBJ_DEAD;
	}

	Act(fTimeDelta);
	m_pTransformCom->Move(m_vInertia);

	m_vInertia.y -= m_fGravity;

	SetUp_OnTerrain(m_pTransformCom, 2.1f);
	return OBJ_NOEVENT;
}

void CBig_worm::Late_Update(_float fTimeDelta)
{
	if (m_iHP <= 0 && m_bDeadSound)
	{
		m_bDeadSound = false;
		m_pGameInstance->PlayDefault(TEXT("BigWorm_Hit.wav"), 27, 0.7f, false);
		m_bDead = true;
	}
	m_pFSM->Late_Update(fTimeDelta);
	Friction(m_fFrictionRate);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	m_pTransformCom->Set_Billboard();
}

HRESULT CBig_worm::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom[m_pFSM->Get_CurState()]->Bind_Texture(m_pTextureCom[m_pFSM->Get_CurState()]->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	_float4x4		ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBig_worm::Ready_Components()
{
	//텍스쳐
	CTexture::TEXTUREDESC   TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.fSpeed = 0.001f;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BigWorm_Idle"),
		TEXT("Com_Texture0", &(int)CFSM::OBJSTATE_IDLE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BigWorm_Move"),
		TEXT("Com_Texture1", &(int)CFSM::OBJSTATE_MOVE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_MOVE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_MOVE, m_pTextureCom[CFSM::OBJSTATE_MOVE]);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BigWorm_Attack"),
		TEXT("Com_Texture2", &(int)CFSM::OBJSTATE_ATTACK), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_ATTACK]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_ATTACK, m_pTextureCom[CFSM::OBJSTATE_ATTACK]);

	//VI버퍼
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return S_OK;

	//트랜스폼
	CTransform::TRANSFORM_DESC  TransformDesc{};
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	m_pPlayer = m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Player"));
	Safe_AddRef(m_pPlayer);
	m_pTransformPlayer = static_cast<CTransform*>(m_pGameInstance->Find_Component(m_eCurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pTransformPlayer);

	/* For.Com_Collider_Cube*/

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 0.5f;
	CollCubeDesc.fRadiusX = 0.5f;
	CollCubeDesc.fRadiusZ = 0.5f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBig_worm::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
}

HRESULT CBig_worm::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CBig_worm::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;

	Desc = { 59, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

	Desc = { 59, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_MOVE, Desc);

	Desc = { 70, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_ATTACK, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

HRESULT CBig_worm::AGGROPlayer()
{
	if (m_fAttackRange < 0)
	{
		MSG_BOX(TEXT("플레이어 인식 거리 오류!"));
		return E_FAIL;
	}

	if (m_fDeltaPlayer <= m_fAttackRange)
	{
		m_eActLevel = ACT_ATTACK;
	}
	else
	{
		m_eActLevel = ACT_AGGRO;
	}

	return S_OK;
}

void CBig_worm::Act(_float _fTimeDelta)
{
	switch (m_eActLevel)
	{
	case CEnemy::ACT_AGGRO:
		break;
	case CEnemy::ACT_ATTACK:
	{
		if (m_fAttackCooltime <= 0)
		{
			CRenderingObject::RENDERINGOBJECT_DESC Desc = { m_pPlayer->Get_Pos() - _float3{0.f, 0.4f, 0.f}, _float3{0.f, 0.f, 1.f}, 1.f, 1.f };
			if (m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_RenderingObject_WarningSign"), &Desc))
			{
				MSG_BOX(TEXT("Failed to Clone WarningSign"));
			}
			m_fAttackCooltime = 1.f;

			CEffect::EFFECT_DESC effDesc;
			effDesc.iFrameSpeed = 0.3f;
			effDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion3");
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &effDesc)))
				MSG_BOX(TEXT("NO EFFECT"));

			m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(Get_Pos());
		}
	}
		break;

	default:
		break;
	}
}

CBig_worm* CBig_worm::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBig_worm* pInstance = new CBig_worm(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create Prototype_CBig_worm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBig_worm::Clone(void* pArg)
{
	CBig_worm* pInstance = new CBig_worm(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone CBig_worm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBig_worm::Free()
{
	__super::Free();
}
