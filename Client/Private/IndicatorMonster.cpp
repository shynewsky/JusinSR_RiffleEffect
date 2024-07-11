#include "stdafx.h"
#include "..\Public\IndicatorMonster.h"

#include "GameInstance.h"


CIndicatorMonster::CIndicatorMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIOrthogonal{ pGraphic_Device }
{
}

CIndicatorMonster::CIndicatorMonster(const CIndicatorMonster & Prototype)
	: CUIOrthogonal{ Prototype }
{
}


HRESULT CIndicatorMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIndicatorMonster::Initialize(void * pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_RenderState(true);

	m_fOriginYPos = m_vWorldPos.y;

	return S_OK;
}

void CIndicatorMonster::Priority_Update(_float fTimeDelta)
{

}

int CIndicatorMonster::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;

	__super::Set_OrthoLH(m_pTransformCom);

	m_fX = max(m_fSizeX*0.5f, m_fX);
	m_fY = max(m_fSizeY*0.5f, m_fY);

	m_fX = min(g_iWinSizeX - m_fSizeX * 0.5f, m_fX);
	m_fY = min(g_iWinSizeY - m_fSizeY * 0.5f, m_fY);
	
	if (m_fZ < 0)
	{
		m_fX < g_iWinSizeX * 0.5f ? m_fX = g_iWinSizeX - m_fSizeX * 0.5f : m_fX = m_fSizeX * 0.5f;
		m_fY < g_iWinSizeY * 0.5f ? m_fY = m_fSizeY * 0.5f : m_fY = g_iWinSizeY - m_fSizeY * 0.5f;
	}

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return OBJ_NOEVENT;
}

void CIndicatorMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	
}

HRESULT CIndicatorMonster::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		

	if (FAILED(__super::SetUp_RenderState()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	if (FAILED(__super::Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}


HRESULT CIndicatorMonster::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_IndicatorMonster"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 2.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIndicatorMonster::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);


	return S_OK;
	
}

HRESULT CIndicatorMonster::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

CIndicatorMonster * CIndicatorMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CIndicatorMonster*		pInstance = new CIndicatorMonster(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CIndicatorMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CIndicatorMonster::Clone(void* pArg)
{
	CIndicatorMonster*		pInstance = new CIndicatorMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CIndicatorMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIndicatorMonster::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

}
