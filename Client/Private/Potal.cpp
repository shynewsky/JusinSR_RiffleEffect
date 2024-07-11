#include "stdafx.h"
#include "..\Public\LogoBG.h"

#include "GameInstance.h"
#include "..\Public\Potal.h"

CPotal::CPotal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CPotal::CPotal(const CPotal& Prototype)
	: CBlendObject{ Prototype }
{
}


HRESULT CPotal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPotal::Initialize(void * pArg)
{
	//Set_Variables(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	POTAL_DESC* Desc = static_cast<POTAL_DESC*>(pArg);
	Set_Pos(Desc->vPos);
	//m_pTransformCom->Scaling(8.0f, 8.0f, 1.f);
	
	return S_OK;
}

void CPotal::Priority_Update(_float fTimeDelta)
{
}

int CPotal::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_pTransformCom->Scaling(m_fScale, m_fScale, 1.f);
	m_fScale += fTimeDelta*2.8f;
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta);
	m_pTransformCom->Go_PosDir(fTimeDelta, _float3(0.f, 0.f, 1.f));

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return OBJ_DEAD;
	m_fTime += fTimeDelta;

	return OBJ_NOEVENT;
}

void CPotal::Late_Update(_float fTimeDelta)
{
	if (m_fTime > 6.f)
		m_bDead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CPotal::Render()
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

void CPotal::Scaling(_float _fScalex, _float _fScaley, _float _fScalez)
{
	m_pTransformCom->Scaling(_fScalex, _fScaley, _fScalez);
}

void CPotal::Lie_On()
{
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), D3DXToRadian(90.f));
}

void CPotal::Set_Variables(void* pArg)
{

}

HRESULT CPotal::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_ChaosPotal"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(-120.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPotal::SetUp_RenderState()
{
	_float3		vSourColor, vDestColor;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CPotal::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

CPotal* CPotal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPotal*		pInstance = new CPotal(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPotal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CPotal::Clone(void* pArg)
{
	CPotal*		pInstance = new CPotal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CPotal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPotal::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
