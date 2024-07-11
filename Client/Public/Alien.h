#pragma once
#include "Enemy.h"

class CAlien :
    public CEnemy
{
protected:
    CAlien(LPDIRECT3DDEVICE9 pGraphic_Device);
    CAlien(const CAlien& Prototype);
    virtual ~CAlien() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual int Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    virtual HRESULT SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY = 0.f) override;

private:
    _float      m_fFrictionRate = { 0.95f };
    _float      m_fGravity = { 0.005f };
    _int        m_iAttackCooltime = { 0 };

private:
    HRESULT Ready_Components();
    HRESULT SetUp_RenderState();
    HRESULT Reset_RenderState();
    HRESULT SetUp_FSM();
    virtual HRESULT AGGROPlayer() override;
    virtual void Act(_float _fTimeDelta) override;

public:
    static CAlien* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

