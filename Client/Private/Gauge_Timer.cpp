#include "stdafx.h"
#include "Gauge_Timer.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Transform.h"

CGauge_Timer::CGauge_Timer(LPDIRECT3DDEVICE9 _pGraphic_Device)
	:CGauge{ _pGraphic_Device }
{
}

CGauge_Timer::CGauge_Timer(const CGauge_Timer& Prototype)
	:CGauge{Prototype}
{
}

HRESULT CGauge_Timer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGauge_Timer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//desc 전달 안하고 쓰려고 레디컴포넌트 함수에 안넣었음
	GAUGE_DESC* Desc = static_cast<GAUGE_DESC*>(pArg);
	CTexture::TEXTUREDESC Texdesc = { Desc->iFrameStart, Desc->iFrameStart, Desc->iFrameEnd, Desc->fFrameSpeed };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Gauge_Timer"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &Texdesc)))
		return E_FAIL;

	m_fCurValue = m_fMaxValue;

	Set_OrthoLH();

	return S_OK;
}

void CGauge_Timer::Priority_Update(_float fTimeDelta){}

int CGauge_Timer::Update(_float fTimeDelta)
{
	if (m_fCurValue <= 0)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CGauge_Timer::Late_Update(_float fTimeDelta)
{
	Adjust_GaugePos();

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
	m_fCurValue -= fTimeDelta;
}

HRESULT CGauge_Timer::Render()
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
	return S_OK;
}

void CGauge_Timer::Set_OrthoLH()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.1f));

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CGauge_Timer::Ready_Components()
{
	//텍스쳐는 부모에서 설정해준다, 한개가 아니거나 디스크립션 안 쓸 경우 따로 맴버 가져가야됨

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

HRESULT CGauge_Timer::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CGauge_Timer::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

void CGauge_Timer::KillSwitch()
{
	m_bDead = true;
}

CGauge_Timer* CGauge_Timer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGauge_Timer* pInstance = new CGauge_Timer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CGauge_Timer"));
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CGauge_Timer::Clone(void* pArg)
{
	CGauge_Timer* pInstance = new CGauge_Timer(*this);

if (FAILED(pInstance->Initialize(pArg)))
{
	MSG_BOX(TEXT("Failed To Clone : CGauge_Timer"));
	Safe_Release(pInstance);
}


return pInstance;
}

void CGauge_Timer::Free()
{
	__super::Free();
}
