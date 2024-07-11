#include "stdafx.h"
#include "Hand.h"
#include "FSM.h"

#include "GameInstance.h"

CHand::CHand(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

CHand::CHand(const CHand& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CHand::Initialize_Prototype()
{
	m_pFSM = nullptr;
	return S_OK;
}

HRESULT CHand::Initialize(void* pArg)
{

	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fX = 320.f;
	m_fY = 638.f;
	m_fZ = 0.15f;

	m_fSizeX = 230.f;
	m_fSizeY = 230.f;

	__super::Set_OrthoLH();

	return S_OK;
}

void CHand::Priority_Update(_float fTimeDelta)
{
}

int CHand::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_NOEVENT;
	return OBJ_NOEVENT;
}

void CHand::Late_Update(_float fTimeDelta)
{
	m_pFSM->Late_Update(fTimeDelta);
}

HRESULT CHand::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pFSM->Get_CurTextureCom()->Bind_Texture(m_pFSM->Get_CurTextureCom()->Get_Frame().iCurrentTex)))
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

void CHand::Reset_Frame()
{
	m_pFSM->Reset_Frame();
}

void CHand::ChangeWeapon()
{
	m_pFSM->Set_State(CFSM::OBJSTATE_TPSCHANGE);
	m_pFSM->Reset_Frame();
}

void CHand::ReloadMotion()
{
	m_pFSM->Set_State(CFSM::OBJSTATE_TPSIDLE);
	m_pFSM->Reset_Frame();
}

HRESULT CHand::Add_RenderObject(CGameObject* pRenderObject)
{
	return m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, pRenderObject);
}

HRESULT CHand::Ready_Components()
{
	/* For.Com_Texture */
#pragma region ÅØ½ºÃÄ

	CTexture::TEXTUREDESC	TextureDesc{ 0 };

	TextureDesc = { 0, 0, 4, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hand_Idle"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_TPSIDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_TPSIDLE, m_pTextureCom[CFSM::OBJSTATE_TPSIDLE]);

	TextureDesc = { 0, 0, 7, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hand_Change"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_TPSCHANGE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_TPSCHANGE, m_pTextureCom[CFSM::OBJSTATE_TPSCHANGE]);

	TextureDesc = { 0, 0, 0, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoRender"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_RENDEROFF]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_RENDEROFF, m_pTextureCom[CFSM::OBJSTATE_RENDEROFF]);

#pragma endregion

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 5.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHand::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);

	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_PrevProj);
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_PrevView);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CHand::Reset_RenderState()
{
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_PrevView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_PrevProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

HRESULT CHand::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 1,1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_RENDEROFF, Desc);
	Desc = { 6, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_TPSIDLE, Desc);
	Desc = { 8, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_TPSCHANGE, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_TPSIDLE);
	return S_OK;
}

CHand* CHand::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHand* pInstance = new CHand(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHand"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHand::Clone(void* pArg)
{
	CHand* pInstance = new CHand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CHand"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHand::Free()
{
	__super::Free();


	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

	CFSM::Destroy(m_pFSM);
}
