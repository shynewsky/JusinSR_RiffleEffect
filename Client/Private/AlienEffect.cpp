#include "stdafx.h"
#include "AlienEffect.h"
#include "GameInstance.h"

CAlienEffect::CAlienEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CRenderingObject{ pGraphic_Device }
{
}

CAlienEffect::CAlienEffect(const CAlienEffect& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CAlienEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAlienEffect::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fX = g_iWinSizeX * 0.5f + (rand() % 500 - 250);
    m_fY = g_iWinSizeY * 0.5f + (rand() % 500 - 500);
    m_fSizeX = 1000.f;
    m_fSizeY = 1000.f;

    Set_OrthoLH();
    m_pGameInstance->PlayDefault(TEXT("AlienHit.wav"), 4, 0.5f, false);
    return S_OK;
}

void CAlienEffect::Priority_Update(_float fTimeDelta)
{

}

int CAlienEffect::Update(_float fTimeDelta)
{
    if (m_pTextureCom->Move_Frame(fTimeDelta, false))
    {
        return OBJ_DEAD;
    }
    m_fY += fTimeDelta * 3;
    Set_OrthoLH();
    return OBJ_NOEVENT;
}

void CAlienEffect::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CAlienEffect::Render()
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

void CAlienEffect::Set_OrthoLH()
{
    D3DXMatrixIdentity(&m_WorldMatrix);
    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixIdentity(&m_ProjMatrix);

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 0.9f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
        -m_fY + g_iWinSizeY * 0.5f, 0.f));

    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}

HRESULT CAlienEffect::Ready_Components()
{
    /* For.Com_Texture */
    CTexture::TEXTUREDESC textureDesc{ 0.f, 0.f, 31.f, 0.1f };
    if (FAILED(__super::Add_Component(LEVEL_SM, TEXT("Prototype_Component_Texture_AlienEffect"),
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

HRESULT CAlienEffect::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 10);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    return S_OK;
}

HRESULT CAlienEffect::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
    
    return S_OK;
}

HRESULT CAlienEffect::SetUp_FSM()
{
    //CFSM::Frame_DESC Desc;
    //Desc = { 31, 1 };
    //m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

    //m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
    return S_OK;
}

CAlienEffect* CAlienEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CAlienEffect* pInstance = new CAlienEffect(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAlienEffect"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CAlienEffect::Clone(void* pArg)
{
    CAlienEffect* pInsatnce = new CAlienEffect(*this);

    if (FAILED(pInsatnce->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CAlienEffect"));
        Safe_Release(pInsatnce);
    }
    return pInsatnce;
}

void CAlienEffect::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
}
