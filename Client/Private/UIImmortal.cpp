#include "stdafx.h"
#include "..\Public\UIImmortal.h"

#include "GameInstance.h"
#include "Player.h"

CUIImmortal::CUIImmortal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CImage{ pGraphic_Device }
{
}

CUIImmortal::CUIImmortal(const CUIImmortal & Prototype)
	: CImage{ Prototype }
{
}

HRESULT CUIImmortal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIImmortal::Initialize(void* pArg)
{
	Set_Variables(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_OrthoLH();

	

	return S_OK;
}

void CUIImmortal::Priority_Update(_float fTimeDelta)
{
}

int CUIImmortal::Update(_float fTimeDelta)
{
	m_fImmotalCount = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Player")))->Get_Immortal();
	return OBJ_NOEVENT;
}

void CUIImmortal::Late_Update(_float fTimeDelta)
{
	if(m_fImmotalCount >= 0.f)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

}

HRESULT CUIImmortal::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture((int)(m_fImmotalCount + 1))))
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

void CUIImmortal::Set_Variables(void* pArg)
{
	IMAGE_DESC* pDesc = static_cast<IMAGE_DESC*>(pArg);

	m_tImageInfo.fX = pDesc->fX;
	m_tImageInfo.fY = pDesc->fY;
	m_tImageInfo.fSizeX = pDesc->fSizeX;
	m_tImageInfo.fSizeY = pDesc->fSizeY;

	m_tImageInfo.eLevelIndex = pDesc->eLevelIndex;
	m_tImageInfo.tFrameTextureTag = pDesc->tFrameTextureTag;
	m_eCurLevel = (LEVELID)pDesc->iFrameCount;
}

void CUIImmortal::Set_OrthoLH()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_pTransformCom->Scaling(m_tImageInfo.fSizeX, m_tImageInfo.fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_tImageInfo.fX - g_iWinSizeX * 0.5f, -m_tImageInfo.fY + g_iWinSizeY * 0.5f, 0.012f));

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

CUIImmortal * CUIImmortal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIImmortal*		pInstance = new CUIImmortal(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIImmortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CUIImmortal::Clone(void* pArg)
{
	CUIImmortal*		pInstance = new CUIImmortal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CUIImmortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIImmortal::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
