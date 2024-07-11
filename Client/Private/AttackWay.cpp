#include "stdafx.h"
#include "..\Public\AttackWay.h"

#include "GameInstance.h"

CAttackWay::CAttackWay(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CAttackWay::CAttackWay(const CAttackWay& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CAttackWay::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CAttackWay::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(ATTACKWAYDESC));

	_float4x4 matRotateX;
	D3DXMatrixRotationX(&matRotateX, D3DXToRadian(90));

	_float4x4 matRotateToTarget;
	D3DXMatrixIdentity(&matRotateToTarget);
	_float3 axis(0, 1, 0); // 회전 축을 설정
	D3DXMatrixRotationAxis(&matRotateToTarget, &axis, atan2f(m_tDesc.fDir.x, m_tDesc.fDir.z));

	_float4x4 matWorld;
	D3DXMatrixMultiply(&matWorld, &matRotateX, &matRotateToTarget);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&matWorld.m[0][0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&matWorld.m[1][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&matWorld.m[2][0]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_tDesc.fX, 0.01f, m_tDesc.fZ));


	m_fViewZ = 50.f;
	return S_OK;
}

void CAttackWay::Priority_Update(_float fTimeDelta) {}

 int CAttackWay::Update(_float fTimeDelta)
{
	if(m_bDead)
		return OBJ_DEAD;

	m_pTransformCom->Go_PosDir(fTimeDelta * 10.f, m_tDesc.fDir);
	m_pTransformCom->Scaling(14.f, 18.f + m_fScale,  1.f);
	m_fSumTime += fTimeDelta;
	m_fScale+= 2.f;
	if (m_fSumTime>=0.5f)
		return OBJ_DEAD;

	/*if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
	{
		return OBJ_NOEVENT;
	}*/

	return S_OK;
}

void CAttackWay::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	return;
}

 HRESULT CAttackWay::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture()))
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


HRESULT CAttackWay::Ready_Components()
{
	/* For.Com_Texture */

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_AttackWay"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 5.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAttackWay::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	return S_OK;
}

HRESULT CAttackWay::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	return S_OK;
}


CAttackWay* CAttackWay::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAttackWay* pInstance = new CAttackWay(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAttackWay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAttackWay::Clone(void* pArg)
{
	CAttackWay* pInstance = new CAttackWay(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CAttackWay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttackWay::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

}
