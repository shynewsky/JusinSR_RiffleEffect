#include "stdafx.h"
#include "Leg.h"
#include "FSM.h"

#include "GameInstance.h"

CLeg::CLeg(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

CLeg::CLeg(const CLeg& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CLeg::Initialize_Prototype()
{
	m_pFSM = nullptr;
	return S_OK;
}

HRESULT CLeg::Initialize(void* pArg)
{
	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fX = 250.f;
	m_fY = 635.f;
	m_fZ = 0.16f;

	m_fSizeX = 170.f;
	m_fSizeY = 138.f;

	__super::Set_OrthoLH();


	return S_OK;
}

void CLeg::Priority_Update(_float fTimeDelta)
{
}

int CLeg::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_NOEVENT;
	return OBJ_NOEVENT;
}

void CLeg::Late_Update(_float fTimeDelta)
{
	m_pFSM->Late_Update(fTimeDelta);
	//if (m_pTargetTransform == nullptr)
	//	return;

	////m_pTransformCom->GOTO(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) +
	////	m_pTargetTransform->Get_State(CTransform::STATE_RIGHT) * -0.72f +
	////	m_pTargetTransform->Get_State(CTransform::STATE_LOOK) * -0.1f);

	////m_pTransformCom->Set_Billboard();
}

HRESULT CLeg::Render()
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

void CLeg::Set_State(CFSM::OBJSTATE _eState)
{
	m_pFSM->Set_State(_eState);
}

HRESULT CLeg::Add_RenderObject(CGameObject* pRenderObject)
{
	return m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, pRenderObject);
}

HRESULT CLeg::Ready_Components()
{
	/* For.Com_Texture */
#pragma region ÅØ½ºÃÄ

	CTexture::TEXTUREDESC	TextureDesc{ 0 };

	TextureDesc = { 0, 0, 4, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Body_Idle"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);

	TextureDesc = { 0, 0, 9, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Body_Move"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_MOVE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_MOVE, m_pTextureCom[CFSM::OBJSTATE_MOVE]);

	TextureDesc = { 0, 0, 8, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Body_BackMove"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_BACKMOVE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_BACKMOVE, m_pTextureCom[CFSM::OBJSTATE_BACKMOVE]);

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

HRESULT CLeg::SetUp_RenderState()
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

HRESULT CLeg::Reset_RenderState()
{
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_PrevView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_PrevProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

HRESULT CLeg::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 8, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_BACKMOVE, Desc);
	Desc = { 4, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);
	Desc = { 9, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_MOVE, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

CLeg* CLeg::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLeg* pInstance = new CLeg(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLeg"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLeg::Clone(void* pArg)
{
	CLeg* pInstance = new CLeg(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLeg"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeg::Free()
{
	__super::Free();


	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

	CFSM::Destroy(m_pFSM);
}
