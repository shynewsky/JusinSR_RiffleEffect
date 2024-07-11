#include "stdafx.h"
#include "Gauge_Skill.h"

#include "GameInstance.h"
//#include "Texture.h"
//#include "Transform.h"
#include "Player.h"

CGauge_Skill::CGauge_Skill(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGauge{ _pGraphic_Device }
{
}

CGauge_Skill::CGauge_Skill(const CGauge_Skill& Prototype)
	:CGauge{ Prototype }
{
}

HRESULT CGauge_Skill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGauge_Skill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//desc 전달 안하고 쓰려고 레디컴포넌트 함수에 안넣었음
	GAUGE_DESC* Desc = static_cast<GAUGE_DESC*>(pArg);

	CTexture::TEXTUREDESC Texdesc = { Desc->iFrameStart, Desc->iFrameStart, Desc->iFrameEnd, Desc->fFrameSpeed };

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkillGauge"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &Texdesc)))
		return E_FAIL;

	Set_OrthoLH();

	return S_OK;
}

void CGauge_Skill::Priority_Update(_float fTimeDelta) { }

int CGauge_Skill::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_fCurValue = min(static_cast<CPlayer*>(m_pTarget)->Get_MP(), m_fMaxValue);

	return OBJ_NOEVENT;
}

void CGauge_Skill::Late_Update(_float fTimeDelta)
{
	Adjust_GaugePos();

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CGauge_Skill::Render()
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

void CGauge_Skill::Set_OrthoLH()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.2f));

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CGauge_Skill::Ready_Components()
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

HRESULT CGauge_Skill::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CGauge_Skill::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

void CGauge_Skill::KillSwitch()
{
	m_bDead = true;
}

CGauge_Skill* CGauge_Skill::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGauge_Skill* pInstance = new CGauge_Skill(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : Gauge_Grenade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGauge_Skill::Clone(void* pArg)
{
	CGauge_Skill* pInstance = new CGauge_Skill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : Gauge_Grenade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGauge_Skill::Free()
{
	__super::Free();
}
