#include "stdafx.h"
#include "Bullet_Corn.h"

#include "GameInstance.h"


CBullet_Corn::CBullet_Corn(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CBullet_Corn::CBullet_Corn(const CBlendObject& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CBullet_Corn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet_Corn::Initialize(void* pArg)
{
	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_vMyLook = pDesc->vMoveDir; // 나아가는 방향

	m_pTransformCom->LookAt(pDesc->vMoveDir); // 플레이어 포지션

	m_pTransformCom->Scaling(2.f, -10.f, 2.f);

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Play3D(TEXT("Nero_IceDrop.wav"), 17, 1.f, vPos, true);

#pragma endregion

	return S_OK;
}

void CBullet_Corn::Priority_Update(_float fTimeDelta)
{
}

int CBullet_Corn::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
#pragma region 테스트코드

		m_pGameInstance->StopSound(17);

#pragma endregion

		return OBJ_DEAD;
	}

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return OBJ_NOEVENT;

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Set_SoundPos(17, vPos);

#pragma endregion

	m_pTransformCom->Go_PosDir(fTimeDelta * 20.f, m_vMyLook);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	m_fCollisionTime += fTimeDelta;

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);
	return OBJ_NOEVENT;
}

void CBullet_Corn::Late_Update(_float fTimeDelta)
{
	if (m_fCollisionTime <= 0.1f)
		return;

	_float3 vDistance = _float3(0, 0, 0);

	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		m_bDead = true;
	}
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		//m_bDead = true;
	}
	else if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < -1.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 50.f)
		m_bDead = true;

	//m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CBullet_Corn::Render()
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

HRESULT CBullet_Corn::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Bullet_Corn"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Corn"),
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
	CollCubeDesc.fRadiusY = 0.3f;
	CollCubeDesc.fRadiusX = 0.3f;
	CollCubeDesc.fRadiusZ = 0.3f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Corn::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);*/

	return S_OK;
}

HRESULT CBullet_Corn::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBullet_Corn* CBullet_Corn::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBullet_Corn* pInstance = new CBullet_Corn(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBullet_Corn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBullet_Corn::Clone(void* pArg)
{
	CBullet_Corn* pInstance = new CBullet_Corn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBullet_Corn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet_Corn::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
