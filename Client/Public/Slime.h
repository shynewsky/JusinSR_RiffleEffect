#pragma once
#include "Enemy_SM.h"

class CSlime :
    public CEnemy_SM
{
public:
    CSlime(LPDIRECT3DDEVICE9 pGraphic_Device);
    CSlime(const CSlime& Prototype);
    virtual ~CSlime() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual int Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();


private:
    _float m_fJumpDleay = 0.f;
    _bool m_bJumpDelay = false;

private:
    HRESULT Ready_Components();
    HRESULT SetUp_RenderState();
    HRESULT Reset_RenderState();
    HRESULT SetUp_FSM() { return S_OK; }


public:
    static CSlime* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

