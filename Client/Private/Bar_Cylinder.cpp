#include "stdafx.h"
#include "Bar_Cylinder.h"

#include "GameInstance.h"

CBar_Cylinder::CBar_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CBar_Cylinder::CBar_Cylinder(const CBlendObject& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CBar_Cylinder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBar_Cylinder::Initialize(void* pArg)
{
	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vDescLook = pDesc->vLookDir;

	m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_vMyLook = { 0.f, 0.f, 0.f };

	m_pTransformCom->Scaling(1.f, 10.f, 1.f);

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Play3D(TEXT("Nero_ClockHand.wav"), 18, 1.f, vPos, true);

#pragma endregion

	return S_OK;
}

void CBar_Cylinder::Priority_Update(_float fTimeDelta)
{
}

int CBar_Cylinder::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
#pragma region 테스트코드

		m_pGameInstance->StopSound(18);

#pragma endregion

		return OBJ_DEAD;
	}

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return OBJ_NOEVENT;

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Set_SoundPos(18, vPos);

#pragma endregion

#pragma region 원기둥 회전

	if (!m_bRotate)
	{
		if (m_dwTime + 3000 < GetTickCount())
		{
			m_bRotate = true;
			m_dwTime = GetTickCount();
		}
	}
	else
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), 0.1f);

		if (m_dwTime + 500 < GetTickCount())
		{
			m_bDead = true;
		}
	}

#pragma endregion

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	m_fCollisionTime += fTimeDelta;

	//m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);
	return OBJ_NOEVENT;
}

void CBar_Cylinder::Late_Update(_float fTimeDelta)
{
	if (m_fCollisionTime <= 0.1f)
		return;

	_float3 vDistance = _float3(0, 0, 0);

	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		m_bDead = true;
	}
	//if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	//{
	//	m_bDead = true;
	//}
	else if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < -1.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 50.f)
		m_bDead = true;

	//m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CBar_Cylinder::Render()
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

HRESULT CBar_Cylinder::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Bar_Cylinder"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cylinder"),
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

HRESULT CBar_Cylinder::SetUp_RenderState()
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

HRESULT CBar_Cylinder::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBar_Cylinder* CBar_Cylinder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBar_Cylinder* pInstance = new CBar_Cylinder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBar_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBar_Cylinder::Clone(void* pArg)
{
	CBar_Cylinder* pInstance = new CBar_Cylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBar_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBar_Cylinder::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
