#include "stdafx.h"
#include "..\Public\LogoBG.h"

#include "GameInstance.h"
#include "..\Public\Effect_No_Billboard.h"

CEffect_No_Billboard::CEffect_No_Billboard(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CEffect_No_Billboard::CEffect_No_Billboard(const CEffect_No_Billboard& Prototype)
	: CBlendObject{ Prototype }
{
}


HRESULT CEffect_No_Billboard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_No_Billboard::Initialize(void * pArg)
{
	Set_Variables(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Scaling(1.5f, 1.5f, 1.f);
	
	return S_OK;
}

void CEffect_No_Billboard::Priority_Update(_float fTimeDelta)
{
}

int CEffect_No_Billboard::Update(_float fTimeDelta)
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

void CEffect_No_Billboard::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CEffect_No_Billboard::Render()
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

void CEffect_No_Billboard::Scaling(_float _fScalex, _float _fScaley, _float _fScalez)
{
	m_pTransformCom->Scaling(_fScalex, _fScaley, _fScalez);
}

void CEffect_No_Billboard::Lie_On()
{
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), D3DXToRadian(90.f));
}

void CEffect_No_Billboard::Set_Variables(void* pArg)
{
	EFFECT_DESC* Desc = static_cast<EFFECT_DESC*>(pArg);

	m_tEffectInfo.tFrameTextureTag = Desc->tFrameTextureTag;
	m_tEffectInfo.iFrameSpeed = Desc->iFrameSpeed;
}

HRESULT CEffect_No_Billboard::Ready_Components()
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

HRESULT CEffect_No_Billboard::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 80);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CEffect_No_Billboard::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

CEffect_No_Billboard* CEffect_No_Billboard::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_No_Billboard*		pInstance = new CEffect_No_Billboard(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_No_Billboard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CEffect_No_Billboard::Clone(void* pArg)
{
	CEffect_No_Billboard*		pInstance = new CEffect_No_Billboard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_No_Billboard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_No_Billboard::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
