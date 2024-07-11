#include "stdafx.h"
#include "..\Public\Torch.h"

#include "GameInstance.h"

CTorch::CTorch(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CTorch::CTorch(const CTorch & Prototype)
	: CGameObject { Prototype }
{
}


HRESULT CTorch::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTorch::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(5.0f, 5.0f, 1.0f);

	return S_OK;
}

void CTorch::Priority_Update(_float fTimeDelta)
{
}

int CTorch::Update(_float fTimeDelta)
{
	m_pTransformCom->Scaling(5.0f, 5.0f, 1.0f);
	m_pTransformCom->Set_Billboard();
	m_pTextureCom->Move_Frame(fTimeDelta);

	return OBJ_NOEVENT;
}

void CTorch::Late_Update(_float fTimeDelta)
{
	
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTorch::Render()
{	

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CTorch::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
}

HRESULT CTorch::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CTorch::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 14;
	TextureDesc.fSpeed = 0.05f;


	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Torch"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &TextureDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
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

CTorch * CTorch::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTorch*		pInstance = new CTorch(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTorch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CTorch::Clone(void* pArg)
{
	CTorch*		pInstance = new CTorch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTorch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTorch::Free()
{
	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
