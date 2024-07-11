#pragma once
#include "Enemy.h"

class CBlue:
    public CEnemy
{
private:
    CBlue(LPDIRECT3DDEVICE9 pGraphic_Device);
    CBlue(const CBlue& Prototype);
    virtual ~CBlue() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual int Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    virtual void IsPicked(_float3* pOutPos, _float* pDist) override;

private:
    _float      m_fFrictionRate = { 0.95f };
    _float      m_fGravity = { 0.01f };
    _float      m_fAttackCooltime = { 0.f };
    _float      m_fRollCooltime = { 10.f };
    _float4x4   m_mCamreraWorld;

private:
    HRESULT Ready_Components();
    HRESULT SetUp_RenderState();
    HRESULT Reset_RenderState();
    HRESULT SetUp_FSM();

private:
    virtual HRESULT AGGROPlayer() override;
    virtual void Act(_float _fTimeDelta) override;
    void CameraAround();


public:
    static CBlue* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

