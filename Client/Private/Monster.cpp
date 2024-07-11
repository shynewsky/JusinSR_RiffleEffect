#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"
#include "ParticleSystem.h"
#include <iostream>

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject { pGraphic_Device }
{
}

CMonster::CMonster(const CMonster & Prototype)
	: CLandObject { Prototype }
{
}


HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(rand() % 20, 0.f, rand() % 20));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(40.f, 0.5f, 65.f));
	SetUp_FSM();

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	
	m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
}

int CMonster::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

	return OBJ_NOEVENT;
}

void CMonster::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pTextureCom[CFSM::OBJSTATE_IDLE]->Move_Frame(fTimeDelta);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	/*_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_BULLET, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{

	}*/
}

HRESULT CMonster::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom[m_pFSM->Get_CurState()]->Bind_Texture(m_pTextureCom[m_pFSM->Get_CurState()]->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}

void CMonster::IsPicked(_float3* pOutPos, _float* pDist)
{
	if (m_bDead)
		return;
	_float3 vHitingPos;
	D3DXVec3TransformCoord(&vHitingPos, pOutPos, &m_pTransformCom->Get_WorldMatrix());

	//m_bDead = true;
	CParticleSystem::STATE_DESC ParticleDesc;
	ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATE_DESC));
	ParticleDesc.eType = CParticleSystem::PARTICLE_TEST;
	ParticleDesc.eTextureLevel = LEVEL_GAMEPLAY;
	ParticleDesc.strTextureTag = TEXT("Prototype_Component_Texture_Particle");
	ParticleDesc.dDuration = 0.4f;
	ParticleDesc.dParticleLifeTime = 1.5f;
	ParticleDesc.dSpawnTime = 1;
	ParticleDesc.fParticlePerSecond = 75;
	ParticleDesc.fVelocityDeviation = 1.f;
	ParticleDesc.iMaxParticleCount = 10;
	ParticleDesc.vParticleScale = _float2(0.5, 0.5);
	ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
	ParticleDesc.iTextureNum = 0;
	ParticleDesc.vVelocity = _float3((rand() % 10) * 0.1f, (rand() % 10) * 0.1f, rand() % 10 * 0.1f);
	ParticleDesc.vPosition = vHitingPos;
	ParticleDesc.vPosition.z += (_float)0.1;
	if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("ProtoType_ParicleSystem"), &ParticleDesc)))
		MSG_BOX(TEXT("no particle"));
}

HRESULT CMonster::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 9;
	TextureDesc.fSpeed = 0.1f;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Idle"),
		TEXT("Com_Texture0", &(int)CFSM::OBJSTATE_IDLE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Move"),
		TEXT("Com_Texture2", &(int)CFSM::OBJSTATE_MOVE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_MOVE]), &TextureDesc)))
		return E_FAIL;

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

HRESULT CMonster::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	/* Blend : 두 색(src:내가 그릴려고하는 픽셀의 색. , dest: 이미 백버퍼에 그려져 있던 색. )을 섞는다. */
	/* 각각의 색들을 어떤 식으로 섞어서 결과를 보여줄건지에대한 설정이 필요하다. */
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);


	return S_OK;
	
}

HRESULT CMonster::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CMonster::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 16, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);
	Desc = { 8, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_DIE, Desc);
	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_MOVE, Desc);
	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_ROLL, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonster*		pInstance = new CMonster(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CMonster::Clone(void* pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	CFSM::Destroy(m_pFSM);
	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
