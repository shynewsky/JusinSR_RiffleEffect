#include "stdafx.h"
#include "RenderingObject.h"
#include "GameInstance.h"

CRenderingObject::CRenderingObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CRenderingObject::CRenderingObject(const CRenderingObject& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CRenderingObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRenderingObject::Initialize(void* pArg)
{
	RENDERINGOBJECT_DESC* pDesc = static_cast<RENDERINGOBJECT_DESC*>(pArg);
	m_fCX = pDesc->fCX;
	m_fCY = pDesc->fCY;
	m_vLook = pDesc->vLook;

	return S_OK;
}

void CRenderingObject::Priority_Update(_float fTimeDelta)
{
}

int CRenderingObject::Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CRenderingObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CRenderingObject::Render()
{
	return S_OK;
}

HRESULT CRenderingObject::Add_RenderObject(CGameObject* pRenderObject)
{
	return m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, pRenderObject);
}

void CRenderingObject::Set_TargetTransform(CTransform* _pTransform)
{
	m_pTargetTransform = _pTransform;
	Safe_AddRef(m_pTargetTransform);
}

void CRenderingObject::Set_OrthoLH()
{
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_fZ));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

}

void CRenderingObject::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pVIBufferCom);

}
