#pragma once
#include "Enemy.h"

class CBoss_Nero final : public CEnemy
{
private:
    CBoss_Nero(LPDIRECT3DDEVICE9 pGraphic_Device);
    CBoss_Nero(const CBoss_Nero& Prototype);
    virtual ~CBoss_Nero() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual int Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

private:
    HRESULT Ready_Components();
    HRESULT SetUp_RenderState();
    HRESULT Reset_RenderState();

private:
    HRESULT SetUp_FSM();
    void CameraAround();
    void Set_BillBoard_Nero();

private:
    HRESULT Set_ObjState();
    void Set_AttackPattern();

private:
    void Fly_Route();
    void Dead_Animation();
    void Greek_Column();
    void Round_Fence();
    void Stone_Stage();

private:
    void Skill_BulletCurtain();
    void Skill_BulletShoot();
    void Skill_Beam();
    void Skill_WallDash();
    void Skill_IceDrop();
    void Skill_Pull();
    void Skill_RazorLine();
    void Skill_ClockHand();

private:
    void Effect_Before_Beam();
    void Effect_Before_Pull();

private:
    _float4x4   m_mCamreraWorld;

    _float m_fMaxFlyHeight = 5.f;
    _float m_fMaxStandHeight = 2.5f;

    _float m_fCurveRate = 4.f; // k상수
    _float m_fDegreeAngle = 0.f; // t상수

    _float m_fSumTime = 0.f;

    DWORD m_dwStateTime = GetTickCount();
    DWORD m_dwPatternTime = GetTickCount();
    DWORD m_dwEffectTime = GetTickCount();

    bool m_bPatternStart = false;
    bool m_bSkillStart = false;
    bool m_bDeadStart = false;

    int m_iEffectCount = 0;
    _bool m_bFinalScript = { false };

public:
    static CBoss_Nero* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

