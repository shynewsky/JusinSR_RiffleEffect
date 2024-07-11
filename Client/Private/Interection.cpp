#include "stdafx.h"
#include "..\Public\Interection.h"

#include "GameInstance.h"
#include "Bullet.h"

CInterection::CInterection(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIOrthogonal{ pGraphic_Device }
{
}

CInterection::CInterection(const CInterection & Prototype)
	: CUIOrthogonal{ Prototype }
{
}


HRESULT CInterection::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInterection::Initialize(void * pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInterection::Priority_Update(_float fTimeDelta)
{
}

int CInterection::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;


	__super::Set_OrthoLH(m_pTransformCom);


	return OBJ_NOEVENT;
}

void CInterection::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	
}

HRESULT CInterection::Render()
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


HRESULT CInterection::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Interection"),
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

HRESULT CInterection::SetUp_RenderState()
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

HRESULT CInterection::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

//void CInterection::Set_OrthoLH()
//{
//	_float4x4 ViewMatrix, ProjMatrix;
//	_float3 ScreenPosition;
//	D3DVIEWPORT9 viewport;
//
//	//m_pGraphic_Device->GetViewport(&viewport);
//	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
//	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
//
//
//	_float3 pos = m_vWorldPos;
//	D3DXVec3TransformCoord(&pos, &pos, &ViewMatrix);
//	D3DXVec3TransformCoord(&pos, &pos, &ProjMatrix);
//	if (pos.z < 0)
//	{
//		//m_bRenderState = false;
//		return;
//	}
//
//
//	_float3 screenPosition;
//	screenPosition.x = ((pos.x + 1.0f) * 0.5f) * 1280;
//	screenPosition.y = ((1.0f - pos.y) * 0.5f) * 720;
//
//
//	m_fX = screenPosition.x;
//	m_fY = screenPosition.y;
//
//	D3DXMatrixIdentity(&m_ViewMatrix);
//	D3DXMatrixIdentity(&m_ProjMatrix);
//
//	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
//		-m_fY + g_iWinSizeY * 0.5f, 0.f));
//
//	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
//}


CInterection * CInterection::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInterection*		pInstance = new CInterection(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CInterection"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CInterection::Clone(void* pArg)
{
	CInterection*		pInstance = new CInterection(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CInterection"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInterection::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

}
