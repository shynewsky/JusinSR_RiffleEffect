#pragma once
#include "Enemy.h"
class CBig_worm:
    public CEnemy
{
protected:
    CBig_worm(LPDIRECT3DDEVICE9 pGraphic_Device);
    CBig_worm(const CBig_worm& Prototype);
    virtual ~CBig_worm() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual int Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

private:
    _float      m_fFrictionRate = { 0.75f };
    _float      m_fGravity = { 0.01f };
    _float      m_fAttackCooltime = { 0.f };

private:
    HRESULT Ready_Components();
    HRESULT SetUp_RenderState();
    HRESULT Reset_RenderState();
    HRESULT SetUp_FSM();
    virtual HRESULT AGGROPlayer() override;
    virtual void Act(_float _fTimeDelta) override;

public:
    static CBig_worm* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

