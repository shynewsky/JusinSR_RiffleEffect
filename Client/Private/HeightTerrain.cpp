#include "stdafx.h"
#include "..\Public\HeightTerrain.h"

#include "GameInstance.h"

CHeightTerrain::CHeightTerrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CHeightTerrain::CHeightTerrain(const CHeightTerrain & Prototype)
	: CGameObject { Prototype }
{
}


HRESULT CHeightTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHeightTerrain::Initialize(void * pArg)
{

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	return S_OK;
}

void CHeightTerrain::Priority_Update(_float fTimeDelta)
{
}

int CHeightTerrain::Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CHeightTerrain::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CHeightTerrain::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		

	_float4x4		ViewMatrix{};	
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);



	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHeightTerrain::Ready_Components(void* pArg)
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if(pArg == nullptr)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Height_Terrain"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
			return E_FAIL;
	}
	else if (*static_cast<int*>(pArg) == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Height_Terrain_Chaos"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
			return E_FAIL;
	}

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CHeightTerrain * CHeightTerrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHeightTerrain*		pInstance = new CHeightTerrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHeightTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CHeightTerrain::Clone(void* pArg)
{
	CHeightTerrain*		pInstance = new CHeightTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CHeightTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHeightTerrain::Free()
{
	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
