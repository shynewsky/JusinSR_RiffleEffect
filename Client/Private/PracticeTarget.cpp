#include "stdafx.h"
#include "..\Public\PracticeTarget.h"

#include "GameInstance.h"
#include "ParticleSystem.h"
#include <iostream>

CPracticeTarget::CPracticeTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject { pGraphic_Device }
{
}

CPracticeTarget::CPracticeTarget(const CPracticeTarget & Prototype)
	: CLandObject { Prototype }
{
}


HRESULT CPracticeTarget::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPracticeTarget::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Scaling(2.f, 2.f, 1.f);
	_float3* vStartPos = static_cast<_float3*>(pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vStartPos);
	m_OriginPos = *vStartPos;
	m_DownMatrix = m_pTransformCom->Get_WorldMatrix();
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CPracticeTarget::Priority_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
}

int CPracticeTarget::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	if (m_fDownTime > 3.f && m_eTargetState == TS_WAIT)
	{
		m_fDownTime = 0.f;
		m_eTargetState = TS_STAND;

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&m_DownMatrix.m[0][0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&m_DownMatrix.m[1][0]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&m_DownMatrix.m[2][0]);
		//m_ReStartPos.y += 0.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ReStartPos);
	}
	else if (m_fDownTime > 1.5f)
	{
		m_eTargetState = TS_WAIT;
	}

	switch (m_eTargetState)
	{
	case TS_STAND:
		m_pTransformCom->Move(m_fSpeed);
		break;

	case TS_DOWN:
		m_pTransformCom->TurnBackwards(fTimeDelta);
		m_fDownTime += fTimeDelta;
		break;
	case TS_WAIT:
		m_fDownTime += fTimeDelta;
		break;
	default:
		break;
	}

	_float3		vPickPos{};
	_float		fDist;


	m_fMoveDistance = D3DXVec3Length(&(m_OriginPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	
	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);


	return OBJ_NOEVENT;
}

void CPracticeTarget::Late_Update(_float fTimeDelta)
{ 
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		m_fSpeed *= -1.f;
		m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}

}

HRESULT CPracticeTarget::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	//m_pColliderCom->Render_ColliderBox();

	return S_OK;
}

void CPracticeTarget::IsPicked(_float3* pOutPos, _float* pDist)
{
	if (m_eTargetState != TS_STAND)
		return;

	m_eTargetState = TS_DOWN;
	m_ReStartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vHitingPos;
	D3DXVec3TransformCoord(&vHitingPos, pOutPos, &m_pTransformCom->Get_WorldMatrix());

	//m_bDead = true;
	CParticleSystem::STATE_DESC ParticleDesc;
	ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATE_DESC));
	ParticleDesc.eType = CParticleSystem::PARTICLE_TEST;
	ParticleDesc.eTextureLevel = LEVEL_GAMEPLAY;
	ParticleDesc.strTextureTag = TEXT("Prototype_Component_Texture_Particle");
	ParticleDesc.dDuration = 0.4f;
	ParticleDesc.dParticleLifeTime = 2.f;
	ParticleDesc.dSpawnTime = 1;
	ParticleDesc.fParticlePerSecond = 75;
	ParticleDesc.fVelocityDeviation = 1.f;
	ParticleDesc.iMaxParticleCount = 20;
	ParticleDesc.vParticleScale = _float2(0.2, 0.2);
	ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
	ParticleDesc.iTextureNum = 0;
	ParticleDesc.vVelocity = _float3((rand() % 10) * 0.1f, (rand() % 10) * 0.1f, rand() % 10 * 0.1f);
	ParticleDesc.vPosition = vHitingPos;
	ParticleDesc.vPosition.z += (_float)0.1;
	if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("ProtoType_ParicleSystem"), &ParticleDesc)))
		MSG_BOX(TEXT("NO PARTICLE"));

	return;
}

HRESULT CPracticeTarget::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PracticeTarget"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
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
	CollCubeDesc.fRadiusZ = 0.1f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPracticeTarget::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);



	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
	
}

HRESULT CPracticeTarget::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CPracticeTarget * CPracticeTarget::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPracticeTarget*		pInstance = new CPracticeTarget(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPracticeTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CPracticeTarget::Clone(void* pArg)
{
	CPracticeTarget*		pInstance = new CPracticeTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CPracticeTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPracticeTarget::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
