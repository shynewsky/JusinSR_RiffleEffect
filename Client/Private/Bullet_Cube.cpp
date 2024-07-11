#include "stdafx.h"
#include "Bullet_Cube.h"
#include "ParticleSystem.h"
#include "GameInstance.h"


CBullet_Cube::CBullet_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
	{
	}

	CBullet_Cube::CBullet_Cube(const CBlendObject& Prototype)
		:CBlendObject{ Prototype }
	{
	}

	HRESULT CBullet_Cube::Initialize_Prototype()
	{
		return S_OK;
	}

	HRESULT CBullet_Cube::Initialize(void* pArg)
	{
		BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

		if (FAILED(Ready_Components()))
			return E_FAIL;
		m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
		
		m_vMyLook = pDesc->vMoveDir; // 나아가는 방향

		m_pTransformCom->LookAt(pDesc->vLookDir); // 플레이어 포지션

		m_pTransformCom->Scaling(4.f, 8.f, 2.f);

		_float4x4 ViewMatrix{};
		m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

#pragma region 테스트코드

		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Play3D(TEXT("Nero_WallDash.wav"), 16, 1.f, vPos, true);

#pragma endregion

		return S_OK;
	}

	void CBullet_Cube::Priority_Update(_float fTimeDelta)
	{
	}

	int CBullet_Cube::Update(_float fTimeDelta)
	{
		if (m_bDead)
		{

			m_pGameInstance->StopSound(16);


			return OBJ_DEAD;
		}

#pragma region 테스트코드

		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Set_SoundPos(16, vPos);

#pragma endregion



		m_pTransformCom->Go_PosDir(fTimeDelta * 10.f, m_vMyLook);

		memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
		m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

		m_fSumTime += fTimeDelta;
		if(m_fSumTime>0.1f)
		{
			m_fSumTime = 0.f;
			_float3 look = m_vMyLook;
			look.y = 0;
			D3DXVec3Normalize(&look, &look);
			_float3 vPos = Get_Pos() + look;
			vPos.y = 0.1f;
			CParticleSystem::STATE_DESC ParticleDesc;
			ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATE_DESC));
			ParticleDesc.eType = CParticleSystem::PARTICLE_TEST;
			ParticleDesc.eTextureLevel = LEVEL_NERO;
			ParticleDesc.strTextureTag = TEXT("Prototype_Component_Texture_EarthParticle");
			ParticleDesc.dDuration = 0.4f;
			ParticleDesc.dParticleLifeTime = 1.5f;
			ParticleDesc.dSpawnTime = 1;
			ParticleDesc.fParticlePerSecond = 75;
			ParticleDesc.fVelocityDeviation = 1.f;
			ParticleDesc.iMaxParticleCount = 20;
			ParticleDesc.vParticleScale = _float2(0.5f, 0.5f);
			ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
			ParticleDesc.iTextureNum = 0;
			ParticleDesc.vVelocity = _float3((rand() % 10) * 0.1f, (rand() % 10) * 0.1f, rand() % 10 * 0.1f);
			ParticleDesc.vPosition = vPos;
			if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_NERO, TEXT("Layer_Particle"), TEXT("ProtoType_ParicleSystem"), &ParticleDesc)))
				MSG_BOX(TEXT("no particle"));
		}

		m_fCollisionTime += fTimeDelta;

		m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);


		return OBJ_NOEVENT;
	}

	void CBullet_Cube::Late_Update(_float fTimeDelta)
	{
		if (m_fCollisionTime <= 0.1f)
			return;

		_float3 vDistance = _float3(0, 0, 0);

		_float3 vPos = Get_Pos();
		if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
		{
			m_bDead = true;
		}
		else if (vPos.y < -1.f || vPos.y > 50.f)
			m_bDead = true;
		else if (vPos.x > 100.f || vPos.x < 0.f)
			m_bDead = true;
		else if (vPos.z > 100.f || vPos.z < 0.f)
			m_bDead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBullet_Cube::Render()
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

	m_pColliderCom->Render_ColliderBox();

	return S_OK;
}

HRESULT CBullet_Cube::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Bullet_Cube"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC TransformDesc{};

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube*/

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 0.5f;
	CollCubeDesc.fRadiusX = 1.f;
	CollCubeDesc.fRadiusZ = 1.f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Cube::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);*/

	return S_OK;
}

HRESULT CBullet_Cube::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBullet_Cube* CBullet_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBullet_Cube* pInstance = new CBullet_Cube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBullet_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBullet_Cube::Clone(void* pArg)
{
	CBullet_Cube* pInstance = new CBullet_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBullet_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet_Cube::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
