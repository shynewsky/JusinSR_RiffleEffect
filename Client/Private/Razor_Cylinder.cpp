#include "stdafx.h"
#include "Razor_Cylinder.h"

#include "GameInstance.h"
#include "Effect.h"
#include "Player.h"

CRazor_Cylinder::CRazor_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CRazor_Cylinder::CRazor_Cylinder(const CBlendObject& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CRazor_Cylinder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRazor_Cylinder::Initialize(void* pArg)
{
	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

#pragma region 콜라이더때문에 트랜스폼 먼저 꺼내서 설정했어

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC TransformDesc{};

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
#pragma endregion

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pTransformCom->Set_State(CTransform::STATE_UP, pDesc->vUpDir);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, pDesc->vRightDir);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, pDesc->vLookDir);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_vMyLook = pDesc->vLookDir;

	m_pTransformCom->Scaling(0.5f, 100.f, 0.5f);

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->PlayDefault(TEXT("Nero_Razor.wav"), 20, 0.3f, true);
	m_fViewZ = 50.f;
#pragma endregion

	return S_OK;
}

void CRazor_Cylinder::Priority_Update(_float fTimeDelta)
{
}

int CRazor_Cylinder::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
#pragma region 테스트코드

		m_pGameInstance->StopSound(20);

#pragma endregion

		return OBJ_DEAD;
	}

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Set_SoundPos(20, vPos);

#pragma endregion

	m_pTransformCom->Go_PosDir(fTimeDelta * 10.f, m_vMyLook);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	m_fCollisionTime += fTimeDelta;

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);
	return OBJ_NOEVENT;
}

void CRazor_Cylinder::Late_Update(_float fTimeDelta)
{
	if (m_fCollisionTime <= 0.1f)
		return;

	_float3 vDistance = _float3(0, 0, 0);

	//if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	//{
	//	m_bDead = true;
	//}
	if (m_bHit)
	{
		m_fSumTime += fTimeDelta;
	}
	if (m_fSumTime > 1.f)
	{
		m_bHit = false;
	}
	if (!m_bHit && m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		m_bHit = true;
		m_fSumTime = 0.f;
		CEffect::EFFECT_DESC effDesc;
		effDesc.iFrameSpeed = 0.3f;
		effDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Electronic");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_NERO, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &effDesc)))
			MSG_BOX(TEXT("NO EFFECT"));

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_NERO, TEXT("Layer_Player")));
		if (!pPlayer)
			return;

		if (pPlayer->Get_Immortal() > 0.f)
			return;

		m_pGameInstance->Get_ObjectList(LEVEL_NERO, TEXT("Layer_Effect"))->back()->Set_Pos(pPlayer->Get_Pos());
	}
	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < -1.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 50.f)
		m_bDead = true;
	if (Get_Pos().x > 120.f || Get_Pos().x < -10.f || Get_Pos().z < -10.f || Get_Pos().z > 120.f)
		m_bDead = true;

	//m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CRazor_Cylinder::Render()
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

	return S_OK;
}

HRESULT CRazor_Cylinder::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Razor_Cylinder"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cylinder"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;



	///* For.Com_Transform */
	//CTransform::TRANSFORM_DESC TransformDesc{};

	//TransformDesc.fSpeedPerSec = 1.f;
	//TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
	//	TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
	//	return E_FAIL;



	/* For.Com_Collider_Cube*/

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 0.3f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;
	/*CollCubeDesc.fRadiusX = 0.3f;
	CollCubeDesc.fRadiusZ = 0.3f;*/
	if (m_pTransformCom->Get_State(CTransform::STATE_LOOK).x == 0)
	{
		CollCubeDesc.fRadiusX = 100.f;
		CollCubeDesc.fRadiusZ = 0.3f;
	}
	else
	{
		CollCubeDesc.fRadiusX = 0.3f;
		CollCubeDesc.fRadiusZ = 100.f;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRazor_Cylinder::SetUp_RenderState()
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

HRESULT CRazor_Cylinder::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CRazor_Cylinder* CRazor_Cylinder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRazor_Cylinder* pInstance = new CRazor_Cylinder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRazor_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRazor_Cylinder::Clone(void* pArg)
{
	CRazor_Cylinder* pInstance = new CRazor_Cylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CRazor_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRazor_Cylinder::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
