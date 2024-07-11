#include "stdafx.h"
#include "ZoomAimShape.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Transform.h"

CZoomAimShape::CZoomAimShape(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CRenderingObject{ pGraphic_Device }
{
}

CZoomAimShape::CZoomAimShape(const CZoomAimShape& Prototype)
    :CRenderingObject{ Prototype }
{
}

HRESULT CZoomAimShape::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CZoomAimShape::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    Set_OrthoLH();

    return S_OK;
}

void CZoomAimShape::Priority_Update(_float fTimeDelta)
{

}

int CZoomAimShape::Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CZoomAimShape::Late_Update(_float fTimeDelta)
{
    Roll(fTimeDelta);
    m_pTextureCom->Move_Frame(fTimeDelta, false);
}

HRESULT CZoomAimShape::Render()
{
    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    D3DXMATRIX Identity = {};
    D3DXMatrixIdentity(&Identity);

    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

    if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    if (FAILED(SetUp_RenderState()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Reset_RenderState()))
        return E_FAIL;
    //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    return S_OK;
}

void CZoomAimShape::Reset_Frame()
{
    m_pTextureCom->Reset_Frame();
}

void CZoomAimShape::Set_OrthoLH()
{
    D3DXMatrixIdentity(&m_WorldMatrix);
    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixIdentity(&m_ProjMatrix);

    m_pTransformCom->Scaling(1000.f, 1000.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-8.f, 0.f, 0.00002f));

    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CZoomAimShape::Ready_Components()
{
    /* For.Com_Texture */
    CTexture::TEXTUREDESC Texdesc = { 0, 0, 8, 0.1f };
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ZoomAimShape"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &Texdesc)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 0.f, 0.f };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CZoomAimShape::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    return S_OK;
}

HRESULT CZoomAimShape::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

    return S_OK;
}

HRESULT CZoomAimShape::Add_RenderObject(CGameObject* pRenderObject)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    return S_OK;
}

void CZoomAimShape::Roll(_float _fTimeDelta)
{
    m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), D3DXToRadian(m_fTheta));
    m_fTheta += 360 * _fTimeDelta * 0.05f;
}

CZoomAimShape* CZoomAimShape::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CZoomAimShape* pInstance = new CZoomAimShape(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CZoomAimShape"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CZoomAimShape::Clone(void* pArg)
{
    CZoomAimShape* pInsatnce = new CZoomAimShape(*this);

    if (FAILED(pInsatnce->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CZoomAimShape"));
        Safe_Release(pInsatnce);
    }
    return pInsatnce;
}

void CZoomAimShape::Free()
{
    __super::Free();
}
