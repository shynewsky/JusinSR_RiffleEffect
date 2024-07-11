#include "stdafx.h"
#include "../Public/ChaosBall.h"

#include "GameInstance.h"
#include "Effect.h"

CChaosBall::CChaosBall(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{ pGraphic_Device }
{
}

CChaosBall::CChaosBall(const CChaosBall& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CChaosBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CChaosBall::Initialize(void* pArg)
{
	ChaosBallDesc* pDesc = static_cast<ChaosBallDesc*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, pDesc->vLook);
	m_vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

    return S_OK;
}

void CChaosBall::Priority_Update(_float fTimeDelta)
{

}

int CChaosBall::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_pTransformCom->Go_PosDir(fTimeDelta * 35.f, m_vMyLook);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	m_fCollisionTime += fTimeDelta;

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);
	return OBJ_NOEVENT;
}

void CChaosBall::Late_Update(_float fTimeDelta)
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
		m_bDead = true;
	}
	else if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y <0.3f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 28.f 
		|| m_pTransformCom->Get_State(CTransform::STATE_POSITION).x < 0.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).x > 150.f
		|| m_pTransformCom->Get_State(CTransform::STATE_POSITION).z < 0.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).z > 150.f)
		m_bDead = true;

	if (m_bDead)
	{
		CEffect::EFFECT_DESC Desc;
		Desc.iFrameSpeed = 0.4f;
		Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_ChaosEffect");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Desc)))
			MSG_BOX(TEXT("NO EFFECT"));

		m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(__super::Get_Pos());
	}


	m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CChaosBall::Render()
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

HRESULT CChaosBall::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_ChaosBall"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

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

HRESULT CChaosBall::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100);

	return S_OK;
}

HRESULT CChaosBall::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CChaosBall* CChaosBall::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CChaosBall* pInstance = new CChaosBall(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChaosBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChaosBall::Clone(void* pArg)
{
	CChaosBall* pInstance = new CChaosBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CChaosBall"));
		Safe_Release(pInstance);
	}

    return pInstance;
}

void CChaosBall::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
