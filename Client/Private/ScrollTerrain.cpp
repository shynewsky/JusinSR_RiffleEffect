#include "stdafx.h"
#include "..\Public\ScrollTerrain.h"

#include "GameInstance.h"

CScrollTerrain::CScrollTerrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CScrollTerrain::CScrollTerrain(const CScrollTerrain & Prototype)
	: CGameObject { Prototype }
{
}


HRESULT CScrollTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScrollTerrain::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CScrollTerrain::Priority_Update(_float fTimeDelta)
{
}

int CScrollTerrain::Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CScrollTerrain::Late_Update(_float fTimeDelta)
{
	D3DXMatrixIdentity(&m_ScrollMatrix);
	// 텍스처 변환 행렬 설정
	
	scrollOffset += m_fDir; // elapsedTime은 프레임 시간(초)입니다.

	/*if (scrollOffset > (D3DX_PI / 8.0f)) m_fDir = -0.01f;
	if (scrollOffset < (-D3DX_PI / 8.0f)) m_fDir = 0.01f;*/

	//D3DXMatrixTranslation(&m_ScrollMatrix, scrollOffset, 0.f, 0.f);
	D3DXMatrixRotationZ(&m_ScrollMatrix, scrollOffset);
	m_pGraphic_Device->SetTransform(D3DTS_TEXTURE0, &m_ScrollMatrix);
	 
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CScrollTerrain::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	return S_OK;
}

HRESULT CScrollTerrain::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CScrollTerrain * CScrollTerrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScrollTerrain*		pInstance = new CScrollTerrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CScrollTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CScrollTerrain::Clone(void* pArg)
{
	CScrollTerrain*		pInstance = new CScrollTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CScrollTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScrollTerrain::Free()
{
	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
