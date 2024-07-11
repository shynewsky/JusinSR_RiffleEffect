#include "stdafx.h"
#include "..\Public\ScreenEffect.h"

#include "GameInstance.h"
#include <iostream>

CScreenEffect::CScreenEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CScreenEffect::CScreenEffect(const CScreenEffect & Prototype)
	: CGameObject { Prototype }
{
}


HRESULT CScreenEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScreenEffect::Initialize(void * pArg)
{


	if (FAILED(Ready_Components()))
		return E_FAIL;

	ZeroMemory(&m_EffectDesc, sizeof(EFFECT_DESC));
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
	m_EffectDesc.eType = pDesc->eType;
	m_EffectDesc.fDuration = pDesc->fDuration;

	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

void CScreenEffect::Priority_Update(_float fTimeDelta)
{
}

int CScreenEffect::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CScreenEffect::Late_Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	m_pShaderCom->Set_RawValue("g_Time", &m_fTime, sizeof(m_fTime));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_SCREEN, this);

	if (m_fTime > m_EffectDesc.fDuration)
		m_bDead = true;
}

HRESULT CScreenEffect::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix()), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &m_ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &m_ProjMatrix), sizeof(_float4x4));

	LPDIRECT3DTEXTURE9 pTexture = NULL;
	LPDIRECT3DSURFACE9 pRenderTargetSurface = NULL; // 렌더 타겟 표면을 저장할 포인터

	// 화면 크기 얻기
	D3DSURFACE_DESC desc;
	m_pGraphic_Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pRenderTargetSurface);
	pRenderTargetSurface->GetDesc(&desc);
	m_pGraphic_Device->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, desc.Format, D3DPOOL_DEFAULT, &pTexture, NULL);

	// 텍스처의 표면 얻기
	LPDIRECT3DSURFACE9 pTextureSurface = NULL;
	pTexture->GetSurfaceLevel(0, &pTextureSurface);

	// 렌더 타겟 데이터를 텍스처 표면으로 복사
	m_pGraphic_Device->StretchRect(pRenderTargetSurface, NULL, pTextureSurface, NULL, D3DTEXF_NONE);

	m_pShaderCom->Set_Texture("g_Texture", pTexture);
	m_pShaderCom->Begin(m_EffectDesc.eType);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End();

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	pTexture->Release();
	pRenderTargetSurface->Release();
	pTextureSurface->Release();
	return S_OK;
}

HRESULT CScreenEffect::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Screen"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
			
	return S_OK;
}

CScreenEffect * CScreenEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScreenEffect*		pInstance = new CScreenEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CScreenEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CScreenEffect::Clone(void* pArg)
{
	CScreenEffect*		pInstance = new CScreenEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CScreenEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScreenEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
