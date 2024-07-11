#include "stdafx.h"
#include "..\Public\Button.h"

#include "GameInstance.h"

CButton::CButton(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CButton::CButton(const CButton & Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton::Initialize(void * pArg)
{
	Set_Variables(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_OrthoLH();

	return S_OK;
}

void CButton::Priority_Update(_float fTimeDelta)
{
}

int CButton::Update(_float fTimeDelta)
{
	ChecK_MouseOver();

	return OBJ_NOEVENT;
}

void CButton::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CButton::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	switch (m_bMouseOver)
	{
	case false:
		if (FAILED(m_pTextureCom->Bind_Texture(m_tButtonInfo.iImgNormal)))
			return E_FAIL;
		m_bSoundOn = false;
		break;
	case true:
		if (!m_bSoundOn)
		{
			m_pGameInstance->PlayDefault(TEXT("Bt_MouseOver.wav"), 15, 0.5f, false);
			m_bSoundOn = true;
		}
		if (FAILED(m_pTextureCom->Bind_Texture(m_tButtonInfo.iImgMouseOver)))
			return E_FAIL;
		break;
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

void CButton::Set_Variables(void* pArg)
{
	BUTTON_DESC* pDesc = static_cast<BUTTON_DESC*>(pArg);

	m_tButtonInfo.fX = pDesc->fX;
	m_tButtonInfo.fY = pDesc->fY;
	m_tButtonInfo.fSizeX = pDesc->fSizeX;
	m_tButtonInfo.fSizeY = pDesc->fSizeY;
	m_tButtonInfo.iImgNormal = pDesc->iImgNormal;
	m_tButtonInfo.iImgMouseOver = pDesc->iImgMouseOver;

	m_tButtonInfo.eLevelIndex = pDesc->eLevelIndex;
	m_tButtonInfo.tFrameTextureTag = pDesc->tFrameTextureTag;
}

void CButton::Set_OrthoLH()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_pTransformCom->Scaling(m_tButtonInfo.fSizeX, m_tButtonInfo.fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, 
		_float3(m_tButtonInfo.fX - g_iWinSizeX * 0.5f, -m_tButtonInfo.fY + g_iWinSizeY * 0.5f, 0.f));

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

void CButton::ChecK_MouseOver()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT rcUI{};
	SetRect(&rcUI, m_tButtonInfo.fX - m_tButtonInfo.fSizeX * 0.5f, m_tButtonInfo.fY - m_tButtonInfo.fSizeY * 0.5f
		, m_tButtonInfo.fX + m_tButtonInfo.fSizeX * 0.5f, m_tButtonInfo.fY + m_tButtonInfo.fSizeY * 0.5f);

	if (true == PtInRect(&rcUI, ptMouse))
		m_bMouseOver = true;
	else
		m_bMouseOver = false;
}

HRESULT CButton::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_tButtonInfo.eLevelIndex, m_tButtonInfo.tFrameTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CButton::SetUp_RenderState()
{
	/* 알파테스트 -> 글자테두리가 반투명해서 실패*/
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CButton::Reset_RenderState()
{
	/* 알파테스트 -> 글자테두리가 반투명해서 실패*/
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

void CButton::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
