#include "stdafx.h"
#include "Search_Scan.h"
#include "FSM.h"
#include "Gauge.h"

#include "GameInstance.h"

CSearch_Scan::CSearch_Scan(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

CSearch_Scan::CSearch_Scan(const CSearch_Scan& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CSearch_Scan::Initialize_Prototype()
{
	m_pFSM = nullptr;
	return S_OK;
}

HRESULT CSearch_Scan::Initialize(void* pArg)
{
	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;
	CRenderingObject::RENDERINGOBJECT_DESC* desc = static_cast<CRenderingObject::RENDERINGOBJECT_DESC*>(pArg);

	m_fCX = desc->fCX;
	m_fCY = desc->fCY;
	m_pTransformCom->GOTO(desc->vPos - _float3{0.f, 0.8f, 0.f});

	m_pPlayer = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"))->front();

	m_pTransformCom->Scaling(m_fCX, m_fCY, 1.f);

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), D3DXToRadian(90.f));


	if (FAILED(Clone_Gauge()))
		return E_FAIL;

	m_fViewZ = 50.f;

	return S_OK;
}

void CSearch_Scan::Priority_Update(_float fTimeDelta)
{
}

int CSearch_Scan::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_NOEVENT;
	return OBJ_NOEVENT;
}

void CSearch_Scan::Late_Update(_float fTimeDelta)
{
	Compute_DeltaPlayer(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CSearch_Scan::Render()
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

void CSearch_Scan::Set_State(CFSM::OBJSTATE _eState)
{
	m_pFSM->Set_State(_eState);
}

HRESULT CSearch_Scan::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC	TextureDesc = { 0, 0, 4, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Search_Scan"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);


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

HRESULT CSearch_Scan::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CSearch_Scan::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

HRESULT CSearch_Scan::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 1, 4 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

void CSearch_Scan::Compute_DeltaPlayer(_float _fTimeDelta)
{
	m_fDeltaPlayer = D3DXVec3Length(&(m_pPlayer->Get_Pos() - Get_Pos()));
	
	if (m_fSearchValue >= m_fMaxValue)
	{
		m_pFSM->Set_Way((WAY)3);
		return;
	}

	if (m_fDeltaPlayer <= m_fCX * 0.5f)
	{
		m_pFSM->Set_Way((WAY)2);
		m_fSearchValue += _fTimeDelta;
	}
	else
	{
		m_pFSM->Set_Way((WAY)1);
	}
}

HRESULT CSearch_Scan::Clone_Gauge()
{

	CGauge::GAUGE_DESC desc = { g_iWinSizeX * 0.5f + 200.f, g_iWinSizeY * 0.5f + 200, 400.f, 10.f, m_fMaxValue, this, 0, 0, 0 };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_UI"), TEXT("Prototype_Gameobject_Gauge_Search_Scan"), &desc)))
		return E_FAIL;
	m_pScan = m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_UI"))->back();


	return S_OK;
}

CSearch_Scan* CSearch_Scan::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSearch_Scan* pInstance = new CSearch_Scan(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSearch_Scan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSearch_Scan::Clone(void* pArg)
{
	CSearch_Scan* pInstance = new CSearch_Scan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CSearch_Scan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSearch_Scan::Free()
{
	__super::Free();


	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

	CFSM::Destroy(m_pFSM);
}
