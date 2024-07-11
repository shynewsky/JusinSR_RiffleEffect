#include "stdafx.h"
#include "..\Public\NextArrow.h"

#include "GameInstance.h"


CNextArrow::CNextArrow(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIOrthogonal{ pGraphic_Device }
{
}

CNextArrow::CNextArrow(const CNextArrow & Prototype)
	: CUIOrthogonal{ Prototype }
{
}


HRESULT CNextArrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNextArrow::Initialize(void * pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Set_RenderState(true);

	m_fOriginYPos = m_vWorldPos.y;

	return S_OK;
}

void CNextArrow::Priority_Update(_float fTimeDelta)
{

}

int CNextArrow::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;

	if (m_bDir)
	{
		m_vWorldPos.y += 3 * fTimeDelta;
		if (m_fOriginYPos + 0.7f <= m_vWorldPos.y)
		{
			m_bDir = !m_bDir;
		}
	}
	if (!m_bDir)
	{
		m_vWorldPos.y -= 3 * fTimeDelta;
		if (m_fOriginYPos - 0.7f >= m_vWorldPos.y)
		{
			m_bDir = !m_bDir;
		}
	}



	__super::Set_OrthoLH(m_pTransformCom);


	return OBJ_NOEVENT;
}

void CNextArrow::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	
}

HRESULT CNextArrow::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		

	if (FAILED(__super::SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(__super::Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}


HRESULT CNextArrow::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_NextArrow"),
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

HRESULT CNextArrow::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);




	return S_OK;
	
}

HRESULT CNextArrow::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

CNextArrow * CNextArrow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CNextArrow*		pInstance = new CNextArrow(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CNextArrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CNextArrow::Clone(void* pArg)
{
	CNextArrow*		pInstance = new CNextArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CNextArrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNextArrow::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

}
