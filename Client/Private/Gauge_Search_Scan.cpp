#include "stdafx.h"
#include "Gauge_Search_Scan.h"
#include "Search_Scan.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Transform.h"

CGauge_Search_Scan::CGauge_Search_Scan(LPDIRECT3DDEVICE9 _pGraphic_Device)
	:CGauge{ _pGraphic_Device }
{
}

CGauge_Search_Scan::CGauge_Search_Scan(const CGauge_Search_Scan& Prototype)
	:CGauge{Prototype}
{
}

HRESULT CGauge_Search_Scan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGauge_Search_Scan::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//desc 전달 안하고 쓰려고 레디컴포넌트 함수에 안넣었음
	GAUGE_DESC* Desc = static_cast<GAUGE_DESC*>(pArg);
	CTexture::TEXTUREDESC Texdesc = { Desc->iFrameStart, Desc->iFrameStart, Desc->iFrameEnd, Desc->fFrameSpeed };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Gauge_Search_Scan"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &Texdesc)))
		return E_FAIL;

	m_fCurValue = 0.f;

	Set_OrthoLH();

	Set_Font();

	return S_OK;
}

void CGauge_Search_Scan::Priority_Update(_float fTimeDelta){}

int CGauge_Search_Scan::Update(_float fTimeDelta)
{
	if (m_fCurValue == m_fMaxValue)
		return OBJ_DEAD;



	return OBJ_NOEVENT;
}

void CGauge_Search_Scan::Late_Update(_float fTimeDelta)
{
	m_fCurValue = static_cast<CSearch_Scan*>(m_pTarget)->Get_SearchValue();
	Adjust_GaugePos();

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CGauge_Search_Scan::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	D3DXMATRIX Identity = {};
	D3DXMatrixIdentity(&Identity);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

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

	RECT rect;
	rect.left = g_iWinSizeX * 0.5f - 70.f;
	rect.right = g_iWinSizeX * 0.5f + 200.f;
	rect.top = g_iWinSizeY * 0.5f - 250.f;
	rect.bottom = g_iWinSizeY * 0.5f - 150.f;

	LPCWSTR text;

	TCHAR Tmp[32];
	swprintf_s(Tmp, L"진행도 : %3d %%", (int)(m_fCurValue * 100.f / m_fMaxValue));

	text = Tmp;

	D3DXCOLOR color = { 0.39f, 1.f, 0.f, 0.5f };
	m_pFont->DrawTextW(nullptr, text, -1, &rect, DT_LEFT, color);

	return S_OK;
}

void CGauge_Search_Scan::Set_OrthoLH()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.1f));

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CGauge_Search_Scan::Ready_Components()
{
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

HRESULT CGauge_Search_Scan::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CGauge_Search_Scan::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

void CGauge_Search_Scan::KillSwitch()
{
	m_bDead = true;
}

HRESULT CGauge_Search_Scan::Set_Font()
{
	LPWSTR fontName = TEXT("Font_search_scan");
	if (FAILED(D3DXCreateFont(m_pGraphic_Device, 30, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, fontName, &m_pFont)))
		return E_FAIL;
	return S_OK;
}

CGauge_Search_Scan* CGauge_Search_Scan::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGauge_Search_Scan* pInstance = new CGauge_Search_Scan(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CGauge_Search_Scan"));
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CGauge_Search_Scan::Clone(void* pArg)
{
	CGauge_Search_Scan* pInstance = new CGauge_Search_Scan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CGauge_Search_Scan"));
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CGauge_Search_Scan::Free()
{
	__super::Free();
}
