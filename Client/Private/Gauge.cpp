#include "stdafx.h"
#include "Gauge.h"
#include "GameInstance.h"


CGauge::CGauge(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CRenderingObject{ pGraphic_Device }
{
}

CGauge::CGauge(const CGauge& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CGauge::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGauge::Initialize(void* pArg)
{
    GAUGE_DESC* Desc = static_cast<GAUGE_DESC*>(pArg);
    m_fX = Desc->fX;
    m_fY = Desc->fY;
    m_fSizeX = Desc->fSizeX;
    m_fSizeY = Desc->fSizeY;
    m_pTarget = Desc->pTarget;
    Safe_AddRef(m_pTarget);
    m_fMaxValue = Desc->fMaxValue;
    m_bDead = false;

    return S_OK;
}

void CGauge::Adjust_GaugePos()
{
   m_pTransformCom->Scaling(max(0.01f, m_fCurValue) * m_fSizeX / m_fMaxValue, m_fSizeY, 1.f);
   m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3{ m_fX - g_iWinSizeX * 0.5f - m_fSizeX + m_fSizeX * 0.5f * (m_fCurValue) / m_fMaxValue ,
                m_fY - g_iWinSizeY * 0.5f, 0.01f });
}

void CGauge::Free()
{
    __super::Free();
    Safe_Release(m_pTarget);
}
