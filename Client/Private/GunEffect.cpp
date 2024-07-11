#include "stdafx.h"
#include "GunEffect.h"
#include "GameInstance.h"

CGunEffect::CGunEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{pGraphic_Device}
{
}

CGunEffect::CGunEffect(const CGunEffect& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CGunEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGunEffect::Initialize(void* pArg)
{

	if (nullptr != pArg)
		memcpy(&m_tDESC, pArg, sizeof(GF_DESC));

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_fX = m_tDESC.vPos.x;
	m_fY = m_tDESC.vPos.y;
	m_fZ = 0.21f;

	m_fSizeX = 256.f;
	m_fSizeY = 256.f;


	m_fViewZ = 0.f;
	__super::Set_OrthoLH();
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian(10));
	return S_OK;
}

void CGunEffect::Priority_Update(_float fTimeDelta)
{
}

int CGunEffect::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;



	m_fFrame += (m_pTextureCom->Get_EndIndex()) * fTimeDelta / 0.15f;
	if (m_fFrame > m_pTextureCom->Get_EndIndex())
	{
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CGunEffect::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CGunEffect::Render()
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

HRESULT CGunEffect::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_tDESC.tFrameTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 0.f, 0.f };

	TransformDesc.fSpeedPerSec = 5.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGunEffect::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_PrevProj);
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_PrevView);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CGunEffect::Reset_RenderState()
{
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_PrevView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_PrevProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	return S_OK;
}

HRESULT CGunEffect::Add_RenderObject(CGameObject* pRenderObject)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, pRenderObject);

	return S_OK;
}

CGunEffect* CGunEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGunEffect* pInstance = new CGunEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGunEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGunEffect::Clone(void* pArg)
{
	CGunEffect* pInstance = new CGunEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGunEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunEffect::Free()
{
	__super::Free();
	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
