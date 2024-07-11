#include "stdafx.h"
#include "..\Public\UIPortrait.h"

#include "GameInstance.h"

CUIPortrait::CUIPortrait(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CUIPortrait::CUIPortrait(const CUIPortrait & Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CUIPortrait::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIPortrait::Initialize(void * pArg)
{
	if (pArg == nullptr)
		m_iTalker = 0;
	else
		m_iTalker = *static_cast<int*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_OrthoLH();

	return S_OK;
}

void CUIPortrait::Priority_Update(_float fTimeDelta)
{
}

int CUIPortrait::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;
	if (m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Portrait")) == nullptr)
		return OBJ_NOEVENT;
	if (m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Portrait"))->front() != this)
		return OBJ_NOEVENT;

	m_pTextureCom->Move_Frame(fTimeDelta);
	return OBJ_NOEVENT;
}

void CUIPortrait::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Portrait")) == nullptr)
		return;
	if (m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Portrait"))->front() != this)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_SCRIPT, this);
}

HRESULT CUIPortrait::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
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

void CUIPortrait::Set_Variables(void* pArg)
{
	//memcpy(&m_tScriptInfo, pArg, sizeof(SCRIPT_DESC));
}

void CUIPortrait::Set_OrthoLH()
{

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	_float m_fSizeX, m_fSizeY, m_fX, m_fY;
	m_fSizeX = 192.f;
	m_fSizeY = 192.f;

	m_fX = 410.f;
	m_fY = 513.f;


	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CUIPortrait::Ready_Components()
{
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	if(m_iTalker == 0)
	{
		TextureDesc.iStartTex = 0;
		TextureDesc.iEndTex = 5;
		TextureDesc.fSpeed = 0.5f;
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PortraitGatto"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &TextureDesc)))
			return E_FAIL;
	}
	else if (m_iTalker == 1)
	{
		TextureDesc.iStartTex = 0;
		TextureDesc.iEndTex = 3;
		TextureDesc.fSpeed = 0.5f;
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PortraitRose"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &TextureDesc)))
			return E_FAIL;
	}
	else if (m_iTalker == 2)
	{
		TextureDesc.iStartTex = 0;
		TextureDesc.iEndTex = 5;
		TextureDesc.fSpeed = 0.5f;
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PortraitBBiz"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &TextureDesc)))
			return E_FAIL;
	}
	else if (m_iTalker == 3)
	{
		TextureDesc.iStartTex = 0;
		TextureDesc.iEndTex = 4;
		TextureDesc.fSpeed = 0.5f;
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PortraitKane"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &TextureDesc)))
			return E_FAIL;
	}
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

HRESULT CUIPortrait::SetUp_RenderState()
{
	/* 알파테스트 -> 글자테두리가 반투명해서 실패*/
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);



	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CUIPortrait::Reset_RenderState()
{

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);


	return S_OK;
}

CUIPortrait* CUIPortrait::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIPortrait* pInstance = new CUIPortrait(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : UIScript"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPortrait::Clone(void* pArg)
{
	CUIPortrait* pInstance = new CUIPortrait(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : UIScript"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPortrait::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
