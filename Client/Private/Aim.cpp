#include "stdafx.h"
#include "Aim.h"
#include "GameInstance.h"

CAim::CAim(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CRenderingObject{ pGraphic_Device }
{
}

CAim::CAim(const CAim& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CAim::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAim::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    AIM_DESC* pDesc = static_cast<AIM_DESC*>(pArg);

    m_fX = pDesc->fX;
    m_fY = pDesc->fY;
    m_fSizeX = pDesc->fSizeX;
    m_fSizeY = pDesc->fSizeY;

    Set_OrthoLH();

    return S_OK;
}

void CAim::Priority_Update(_float fTimeDelta)
{

}

int CAim::Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CAim::Late_Update(_float fTimeDelta)
{
}

HRESULT CAim::Render()
{
    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;
    
    D3DXMATRIX Identity = {};
    D3DXMatrixIdentity(&Identity);

    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture()))
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

void CAim::Reset_Frame()
{
    m_pFSM->Reset_Frame();
}

void CAim::Set_State(CFSM::OBJSTATE _eState)
{
    m_pFSM->Set_State(_eState);
}

void CAim::Set_OrthoLH()
{
    D3DXMatrixIdentity(&m_WorldMatrix);
    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixIdentity(&m_ProjMatrix);

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
        -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CAim::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Aim"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
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

HRESULT CAim::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

    return S_OK;
}

HRESULT CAim::Reset_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
    
    return S_OK;
}

HRESULT CAim::Add_RenderObject(CGameObject* pRenderObject)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, pRenderObject);
    return S_OK;
}

HRESULT CAim::SetUp_FSM()
{
    CFSM::Frame_DESC Desc;
    Desc = { 1, 2 };
    m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

    m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
    return S_OK;
}

CAim* CAim::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CAim* pInstance = new CAim(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAim"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CAim::Clone(void* pArg)
{
    CAim* pInsatnce = new CAim(*this);

    if (FAILED(pInsatnce->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CAim"));
        Safe_Release(pInsatnce);
    }
    return pInsatnce;
}

void CAim::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
}
