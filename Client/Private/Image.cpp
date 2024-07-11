#include "stdafx.h"
#include "..\Public\Image.h"

#include "GameInstance.h"

CImage::CImage(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CImage::CImage(const CImage& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CImage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CImage::Initialize(void* pArg)
{
	Set_Variables(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_OrthoLH();

	return S_OK;
}

void CImage::Priority_Update(_float fTimeDelta)
{
}

int CImage::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_fFrame += fTimeDelta * m_tImageInfo.iFrameSpeed;

	if (m_fFrame >= m_tImageInfo.iFrameCount)
		m_fFrame = 0.f;

	return OBJ_NOEVENT;
}

void CImage::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CImage::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	//_float4x4 ViewMatrix{};
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (m_tImageInfo.iFrameCount == 1)
	{
		if (FAILED(m_pTextureCom->Bind_Texture(0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame)))
			return E_FAIL;
	}

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

void CImage::Set_Variables(void* pArg)
{
	IMAGE_DESC* pDesc = static_cast<IMAGE_DESC*>(pArg);

	m_tImageInfo.fX = pDesc->fX;
	m_tImageInfo.fY = pDesc->fY;
	m_tImageInfo.fSizeX = pDesc->fSizeX;
	m_tImageInfo.fSizeY = pDesc->fSizeY;

	m_tImageInfo.eLevelIndex = pDesc->eLevelIndex;
	m_tImageInfo.tFrameTextureTag = pDesc->tFrameTextureTag;
	m_tImageInfo.iFrameCount = pDesc->iFrameCount;
	m_tImageInfo.iFrameSpeed = pDesc->iFrameSpeed;
}

void CImage::Set_OrthoLH()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);
	
	m_pTransformCom->Scaling(m_tImageInfo.fSizeX, m_tImageInfo.fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, 
		_float3(m_tImageInfo.fX - g_iWinSizeX * 0.5f, -m_tImageInfo.fY + g_iWinSizeY * 0.5f, 0.01f));

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CImage::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_tImageInfo.eLevelIndex, m_tImageInfo.tFrameTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc{ 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CImage::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return S_OK;
}

HRESULT CImage::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	return E_NOTIMPL;
}

void CImage::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
