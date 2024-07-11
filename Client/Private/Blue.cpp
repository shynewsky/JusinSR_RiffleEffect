#include "stdafx.h"
#include "Blue.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Player.h"
#include "Bullet.h"
#include "Effect.h"

CBlue::CBlue(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CEnemy{ pGraphic_Device }
{
}

CBlue::CBlue(const CBlue& Prototype)
	:CEnemy{ Prototype }
{
}

HRESULT CBlue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlue::Initialize(void* pArg)
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
	m_fFrictionRate = 0.85;
	return S_OK;
}

void CBlue::Priority_Update(_float fTimeDelta)
{
	if (m_iHP <= 0)
		return;
	m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
	Compute_DeltaPlayer();
	AGGROPlayer();
	repulsion();
}

int CBlue::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
		CEffect::EFFECT_DESC effDesc;
		effDesc.iFrameSpeed = 0.3f;
		effDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion10");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &effDesc)))
			MSG_BOX(TEXT("NO EFFECT"));

		m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(Get_Pos());
		return OBJ_DEAD;
	}
	else if (m_iHP <= 0)
		return OBJ_NOEVENT;

	Act(fTimeDelta); 

	m_pTransformCom->Move(m_vInertia); 

	m_vInertia.y -= m_fGravity; 

	SetUp_OnTerrain(m_pTransformCom, 0.5f); 

	return OBJ_NOEVENT;
}

void CBlue::Late_Update(_float fTimeDelta)
{
	if (OBJ_DEAD == m_pFSM->Late_Update(fTimeDelta))
		m_bDead = true;
	if (m_iHP <= 0 && m_bDeadSound)
	{
		m_bDeadSound = false;
		m_pGameInstance->PlayDefault(TEXT("Blue_Hit.wav"), 27, 0.7f, false);
		m_pFSM->Set_State(CFSM::OBJSTATE_DIE);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

		return;
	}
	Friction(m_fFrictionRate);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	CameraAround(); 

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));

	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix); 


	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);

	m_pTransformCom->Set_Billboard();
	m_fRollCooltime -= fTimeDelta;
}

HRESULT CBlue::Render()
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

void CBlue::IsPicked(_float3* pOutPos, _float* pDist)
{
	if (m_bDead)
		return;


	_float2 vPos = _float2(pOutPos->x, pOutPos->y);
	if (D3DXVec2Length(&vPos) >= 0.43f)
		return;

	_float3 vHitingPos;
	D3DXVec3TransformCoord(&vHitingPos, pOutPos, &m_pTransformCom->Get_WorldMatrix());

	CEffect::EFFECT_DESC Desc;

	CPlayer* pPlayer = nullptr;

	pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	if (pPlayer == nullptr)
		return;

	if (pPlayer->Get_Weapon() == CPlayer::WEAPONS::RAMBO)
	{
		Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_RifleEffect");
		m_iHP--;
	}

	else if (pPlayer->Get_Weapon() == CPlayer::WEAPONS::SPARROW)
	{
		Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_SniperEffect");
		m_iHP -= 5;
	}
		

	Desc.iFrameSpeed = 0.3f;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Desc)))
		MSG_BOX(TEXT("NO EFFECT"));

	m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(vHitingPos);
	static_cast<CPlayer*>(m_pPlayer)->Plus_Fever(0.1f);



}

HRESULT CBlue::Ready_Components()
{
	//텍스쳐
	CTexture::TEXTUREDESC   TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Move"),
		TEXT("Com_Texture0", &(int)CFSM::OBJSTATE_MOVE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_MOVE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_MOVE, m_pTextureCom[CFSM::OBJSTATE_MOVE]);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Die"),
		TEXT("Com_Texture1", &(int)CFSM::OBJSTATE_DIE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_DIE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_DIE, m_pTextureCom[CFSM::OBJSTATE_DIE]);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Roll"),
		TEXT("Com_Texture2", &(int)CFSM::OBJSTATE_ROLL), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_ROLL]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_ROLL, m_pTextureCom[CFSM::OBJSTATE_ROLL]);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Idle"),
		TEXT("Com_Texture3", &(int)CFSM::OBJSTATE_IDLE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);

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

HRESULT CBlue::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 250);

	return S_OK;
}

HRESULT CBlue::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CBlue::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_MOVE, Desc);

	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_ROLL, Desc);

	Desc = { 8, 1 }; 
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_DIE, Desc);

	Desc = { 16,8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

HRESULT CBlue::AGGROPlayer()
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

void CBlue::Act(_float _fTimeDelta)
{
	D3DXVec3Normalize(&m_vDeltaPlayer, &m_vDeltaPlayer);

	switch (m_eActLevel)
	{
	case CEnemy::ACT_AGGRO:
		m_vInertia += m_vDeltaPlayer * _fTimeDelta;
		m_pFSM->Set_State(CFSM::OBJSTATE_MOVE);
		break;
	case CEnemy::ACT_ATTACK:
		if (m_fAttackCooltime <= 0)
		{
			m_fAttackCooltime = 2.f;
			CBullet::BulletDesc Desc = { Get_Pos(), m_vDeltaPlayer};
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Bullet"), &Desc)))
				return;
			m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
		}
		else
			m_fAttackCooltime -= _fTimeDelta;

		if (m_fRollCooltime <= 0)
		{
			D3DXVec3Normalize(&m_vDeltaPlayer, &m_vDeltaPlayer);
			m_vInertia += m_vDeltaPlayer;
			m_pFSM->Set_State(CFSM::OBJSTATE_ROLL);
			m_fRollCooltime = 10.f;
		}
		else if (CFSM::OBJSTATE_MOVE == m_pFSM->Get_CurState())
			m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
		break;
	default:
		break;
	}
}

void CBlue::CameraAround()
{
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_mCamreraWorld);

	D3DXMatrixInverse(&m_mCamreraWorld, nullptr, &m_mCamreraWorld);

	//cos22.5 = 0.923880
	//cos67.5 = 0.382683
	_float zDot = D3DXVec3Dot(&m_vDeltaPlayer, &(*(_float3*)(&m_mCamreraWorld._31)));

	if (zDot >= 0.9238)
	{
		m_pFSM->Turn_Frame(WAY_BACK);
		return;
	}
	else if (zDot < -0.9238)
	{
		m_pFSM->Turn_Frame(WAY_FRONT);
		return;
	}

	_float xDot = D3DXVec3Dot(&m_vDeltaPlayer, &(*(_float3*)(&m_mCamreraWorld._11)));

	if (zDot >= 0.3826)
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

	if (zDot < -0.3826)
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

CBlue* CBlue::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlue* pInstance = new CBlue(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("failed to Create Prototype Blue"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlue::Clone(void* pArg)
{
	CBlue* pInstance = new CBlue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("failed to Clone Blue"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlue::Free()
{

	__super::Free();

}
