#include "stdafx.h"
#include "..\Public\GrenadeRange.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Vibuffer_Sphere.h"

CGrenadeRange::CGrenadeRange(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{pGraphic_Device}
{

}

CGrenadeRange::CGrenadeRange(const CGrenadeRange& Prototype)
	:CBlendObject{Prototype}
{
}

HRESULT CGrenadeRange::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGrenadeRange::Initialize(void* pArg)
{
	RANGE_DESC Desc = *static_cast<RANGE_DESC*>(pArg);
	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_Pos(Desc.vPos);
	m_pTransformCom->Scaling(1.5f, 1.5f, 1.5f);
	Ready_Components();
	return S_OK;
}

void CGrenadeRange::Priority_Update(_float fTimeDelta)
{

}

int CGrenadeRange::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
	{
		MSG_BOX(TEXT("알파소팅 불가!"));
		return OBJ_NOEVENT;
	}
	
	return OBJ_NOEVENT;
}

void CGrenadeRange::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CGrenadeRange::Render()
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

void CGrenadeRange::KillSwitch()
{
	m_bDead = true;
}

HRESULT CGrenadeRange::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GrenadeRange"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Sphere"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrenadeRange::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CGrenadeRange::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

CGrenadeRange* CGrenadeRange::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGrenadeRange* pInstance = new CGrenadeRange(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGrenadeRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrenadeRange::Clone(void* pArg)
{
	CGrenadeRange* pInstance = new CGrenadeRange(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGrenadeRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrenadeRange::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

}
