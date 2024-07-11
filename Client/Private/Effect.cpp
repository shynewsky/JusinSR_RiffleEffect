#include "stdafx.h"
#include "..\Public\LogoBG.h"

#include "GameInstance.h"
#include "..\Public\Effect.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CEffect::CEffect(const CEffect & Prototype)
	: CBlendObject{ Prototype }
{
}


HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{
	Set_Variables(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Scaling(1.5f, 1.5f, 1.f);
	
	return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
}

int CEffect::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_fFrame += (m_pTextureCom->Get_EndIndex()) * fTimeDelta / m_tEffectInfo.iFrameSpeed;

	if (m_fFrame > m_pTextureCom->Get_EndIndex())
	{
		m_fFrame = 0.f;
		return OBJ_DEAD;
	}

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame)))
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

void CEffect::Scaling(_float _fScalex, _float _fScaley, _float _fScalez)
{
	m_pTransformCom->Scaling(_fScalex, _fScaley, _fScalez);
}

void CEffect::Lie_On()
{
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), D3DXToRadian(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3{ Get_Pos().x, 0.1f, Get_Pos().z });
}

void CEffect::Set_Variables(void* pArg)
{
	EFFECT_DESC* Desc = static_cast<EFFECT_DESC*>(pArg);

	m_tEffectInfo.tFrameTextureTag = Desc->tFrameTextureTag;
	m_tEffectInfo.iFrameSpeed = Desc->iFrameSpeed;
}

HRESULT CEffect::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_tEffectInfo.tFrameTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::SetUp_RenderState()
{
	_float3		vSourColor, vDestColor;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CEffect::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

CEffect * CEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect*		pInstance = new CEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CEffect::Clone(void* pArg)
{
	CEffect*		pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
