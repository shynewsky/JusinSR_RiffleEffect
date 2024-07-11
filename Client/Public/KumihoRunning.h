#pragma once
#include "Enemy_SM.h"

class CKumihoRunning :
    public CEnemy_SM
{
public:
    CKumihoRunning(LPDIRECT3DDEVICE9 pGraphic_Device);
    CKumihoRunning(const CKumihoRunning& Prototype);
    virtual ~CKumihoRunning() = default;

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
    static CKumihoRunning* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

