#include "stdafx.h"
#include "ChronoEffect.h"
#include "GameInstance.h"

CChronoEffect::CChronoEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CRenderingObject{ pGraphic_Device }
{
}

CChronoEffect::CChronoEffect(const CChronoEffect& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CChronoEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CChronoEffect::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    //AIM_DESC* pDesc = static_cast<AIM_DESC*>(pArg);

    //m_fX = pDesc->fX;
    //m_fY = pDesc->fY;
    //m_fSizeX = pDesc->fSizeX;
    //m_fSizeY = pDesc->fSizeY;

    m_fX = g_iWinSizeX * 0.5f;
    m_fY = g_iWinSizeY * 0.5f;
    m_fSizeX = 200.f;
    m_fSizeY = 200.f;

    Set_OrthoLH();

    m_pGameInstance->PlayDefault(TEXT("slowdown.wav"), 2, 1.f, true);
    return S_OK;
}

void CChronoEffect::Priority_Update(_float fTimeDelta)
{

}

int CChronoEffect::Update(_float fTimeDelta)
{
    if (m_pTextureCom->Move_Frame(fTimeDelta, false))
    {
        //m_pGameInstance->StopSound(2);
        //m_pGameInstance->Play_NonStack(TEXT("magnum_reload.wav"), 2, 1.f, false);
        return OBJ_DEAD;
    }
    
    return OBJ_NOEVENT;
}

void CChronoEffect::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CChronoEffect::Render()
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

    if (FAILED(SetUp_RenderState()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Reset_RenderState()))
        return E_FAIL;

    return S_OK;
}

void CChronoEffect::Set_OrthoLH()
{
    D3DXMatrixIdentity(&m_WorldMatrix);
    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixIdentity(&m_ProjMatrix);

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
        -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CChronoEffect::Ready_Components()
{
    /* For.Com_Texture */
    CTexture::TEXTUREDESC textureDesc{ 0.f, 0.f, 8.f, 0.1f };
    if (FAILED(__super::Add_Component(LEVEL_SM, TEXT("Prototype_Component_Texture_ChronoEffect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &textureDesc)))
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

HRESULT CChronoEffect::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 50);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    return S_OK;
}

HRESULT CChronoEffect::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
    
    return S_OK;
}

HRESULT CChronoEffect::SetUp_FSM()
{
    CFSM::Frame_DESC Desc;
    Desc = { 8, 1 };
    m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

    m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
    return S_OK;
}

CChronoEffect* CChronoEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CChronoEffect* pInstance = new CChronoEffect(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CChronoEffect"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CChronoEffect::Clone(void* pArg)
{
    CChronoEffect* pInsatnce = new CChronoEffect(*this);

    if (FAILED(pInsatnce->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CChronoEffect"));
        Safe_Release(pInsatnce);
    }
    return pInsatnce;
}

void CChronoEffect::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
}
