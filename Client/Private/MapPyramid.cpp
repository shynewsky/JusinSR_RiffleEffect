#include "stdafx.h"
#include "..\Public\MapPyramid.h"

#include "GameInstance.h"
#include <DirectXCollision.h> 

CMapPyramid::CMapPyramid(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CMapPyramid::CMapPyramid(const CMapPyramid & Prototype)
	: CGameObject { Prototype }
{
}


HRESULT CMapPyramid::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapPyramid::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(50.f, 2.f, 20.f));

	return S_OK;
}

void CMapPyramid::Priority_Update(_float fTimeDelta)
{
}

int CMapPyramid::Update(_float fTimeDelta)
{


	return OBJ_NOEVENT;
}

void CMapPyramid::Late_Update(_float fTimeDelta)
{
	
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMapPyramid::Render()
{	

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_Texture(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		

	__super::Render();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


	return S_OK;
}

HRESULT CMapPyramid::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Wall"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Pyramid"),
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

CMapPyramid * CMapPyramid::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapPyramid*		pInstance = new CMapPyramid(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMapPyramid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CMapPyramid::Clone(void* pArg)
{
	CMapPyramid*		pInstance = new CMapPyramid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMapPyramid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapPyramid::Free()
{
	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
