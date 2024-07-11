#include "stdafx.h"
#include "ZoomAim.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

CZoomAim::CZoomAim(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

CZoomAim::CZoomAim(const CZoomAim& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CZoomAim::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CZoomAim::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    Set_OrthoLH();

    m_pTextureCom->Set_FrameSpeed(0.01f);

    return S_OK;
}

void CZoomAim::Priority_Update(_float fTimeDelta)
{

}

int CZoomAim::Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CZoomAim::Late_Update(_float fTimeDelta)
{
    m_pTextureCom->Move_Frame(fTimeDelta, false);
}

HRESULT CZoomAim::Render()
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

void CZoomAim::Reset_Frame()
{
    m_pTextureCom->Reset_Frame();
}

void CZoomAim::Set_OrthoLH()
{
    D3DXMatrixIdentity(&m_WorldMatrix);
    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixIdentity(&m_ProjMatrix);

    m_pTransformCom->Scaling(1000.f, 1000.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-8.f, 0.f, 0.0002f));

    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CZoomAim::Ready_Components()
{
    /* For.Com_Texture */
    CTexture::TEXTUREDESC Texdesc = { 0, 0, 25, 0.1f };
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ZoomAim"),
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

HRESULT CZoomAim::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    return S_OK;
}

HRESULT CZoomAim::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

    return S_OK;
}

HRESULT CZoomAim::Add_RenderObject(CGameObject* pRenderObject)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, pRenderObject);

    return S_OK;
}

CZoomAim* CZoomAim::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CZoomAim* pInstance = new CZoomAim(pGraphic_Device);
    
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CZoomAim"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CZoomAim::Clone(void* pArg)
{
    CZoomAim* pInsatnce = new CZoomAim(*this);

    if (FAILED(pInsatnce->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CZoomAim"));
        Safe_Release(pInsatnce);
    }
    return pInsatnce;
}

void CZoomAim::Free()
{
    __super::Free();
}
