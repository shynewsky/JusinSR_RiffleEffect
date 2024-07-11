#include "stdafx.h"
#include "..\Public\UIScript.h"

#include "GameInstance.h"
#include "UIPortrait.h"


CUIScript::CUIScript(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CUIScript::CUIScript(const CUIScript & Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CUIScript::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIScript::Initialize(void * pArg)
{
	if(FAILED(Set_Variables(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_OrthoLH();

	return S_OK;
}

void CUIScript::Priority_Update(_float fTimeDelta)
{
}

int CUIScript::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;
	if (m_pGameInstance->Get_ObjectList(m_eCurrentLevel, TEXT("Layer_Script")) == nullptr)
		return OBJ_NOEVENT;
	if (m_pGameInstance->Get_ObjectList(m_eCurrentLevel, TEXT("Layer_Script"))->front() != this)
		return OBJ_NOEVENT;

	if (!m_bSound)
	{
		m_pGameInstance->StopSound(25);

		if (m_iTalker == 0)
			m_pGameInstance->PlayDefault(L"Gatto_Talk.wav", 25, 1.0f, false);
		else if (m_iTalker == 1)
			m_pGameInstance->PlayDefault(L"Rose_Talk.wav", 25, 1.0f, false);
		else if (m_iTalker == 2)
			m_pGameInstance->PlayDefault(L"BBiz_Talk.wav", 25, 1.0f, false);
		else if (m_iTalker == 3)
			m_pGameInstance->PlayDefault(L"Kane_Talk.wav", 25, 01.0f, false);
		m_bSound = true;
	}

	m_fSumtime += fTimeDelta;
	if (m_fSumtime>=0.1f)
	{
		m_fSumtime = 0.f;
		m_CurrentCharIndex++;

		if (m_CurrentCharIndex > m_strFullText.size())
		{
			m_CurrentCharIndex = m_strFullText.size(); // 텍스트가 끝에 도달하면 멈춤
			m_pGameInstance->StopSound(25);
			m_EnterShow = 1;
		}
	}

	if (m_pGameInstance->GetButtonDown(KeyType::Enter))
	{
		if (m_EnterShow == 0)
		{
			m_EnterShow = 1;
			m_CurrentCharIndex = m_strFullText.size();
		}
		else
		{
			m_bDead = true;
			m_pGameInstance->StopSound(25);
			m_pMyPortrait->Set_Dead(true);
		}
		
	}

	return OBJ_NOEVENT;
}

void CUIScript::Late_Update(_float fTimeDelta)
{
	if(m_pGameInstance->Get_ObjectList(m_eCurrentLevel, TEXT("Layer_Script")) == nullptr)
		return;
	if (m_pGameInstance->Get_ObjectList(m_eCurrentLevel, TEXT("Layer_Script"))->front() != this)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIScript::Render()
{	
	if (m_pGameInstance->Get_ObjectList(m_eCurrentLevel, TEXT("Layer_Script")) == nullptr)
		return S_OK;
	if (m_pGameInstance->Get_ObjectList(m_eCurrentLevel, TEXT("Layer_Script"))->front() != this)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(m_EnterShow)))
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
	SetRect(&rect, 540, 430, 1000, 780);
	std::wstring textToDraw = m_strFullText.substr(0, m_CurrentCharIndex);
	m_pGameInstance->Get_Font()->DrawTextW(nullptr, textToDraw.c_str(), -1, &rect, 0, m_dwColor);


	return S_OK;
}

HRESULT CUIScript::Set_Variables(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	SCRIPT_DESC* pDesc = static_cast<SCRIPT_DESC*>(pArg);

	m_strFullText = pDesc->Script;
	m_eCurrentLevel = pDesc->eLevel;
	m_iTalker = pDesc->iTalker;


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurrentLevel, TEXT("Layer_Portrait"),
		TEXT("Prototype_GameObject_UIPortrait"), &m_iTalker)))
		return E_FAIL;

	m_pMyPortrait = m_pGameInstance->Get_ObjectList(m_eCurrentLevel, TEXT("Layer_Portrait"))->back();
	return S_OK;
}

void CUIScript::Set_OrthoLH()
{

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	_float m_fSizeX, m_fSizeY, m_fX, m_fY;
	m_fSizeX = 833.f;
	m_fSizeY = 348.0f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY - m_fSizeY * 0.5f;
	m_fY -= 30;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.001f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CUIScript::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ScriptPannel"),
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

HRESULT CUIScript::SetUp_RenderState()
{
	/* 알파테스트 -> 글자테두리가 반투명해서 실패*/
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CUIScript::Reset_RenderState()
{
	/* 알파테스트 -> 글자테두리가 반투명해서 실패*/
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

CUIScript* CUIScript::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIScript* pInstance = new CUIScript(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : UIScript"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIScript::Clone(void* pArg)
{
	CUIScript* pInstance = new CUIScript(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : UIScript"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIScript::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
