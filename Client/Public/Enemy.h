#pragma once
#include "LandObject.h"
#include "FSM.h"

/*
몬스터 생성 메모
Priority_Update에 들어갈 함수
1. Add_PickintGroup
2. Compute_DeltaPlayer
3. AGGROPlayer

Update 에 들어갈 함수
Act
관성
중력
SetUp_OnTerrain

Late_Update에 들어갈 함수
FSM
마찰력
Set_Billboard

*/




BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

class CEnemy abstract:
    public CLandObject
{
public:
	enum ACTLEVEL { ACT_UNRECOGNIZE, ACT_AGGRO, ACT_ATTACK };
    typedef struct
    {
        CVIBuffer_Terrain* pTerrainVIBuffer = { nullptr };
        CTransform* pTerrainTransform = { nullptr };
        LEVELID					eLevel = { LEVEL_GAMEPLAY };

        _float3  fScale = { 1.f,1.f,1.f }; //xyz 스케일링값
        _float3 vDir;           //생성위치
        _int    iMaxHP = 0;
    }ENEMY_DESC;
protected:
    CEnemy(LPDIRECT3DDEVICE9 pGraphic_Device);
    CEnemy(const CEnemy& Prototype);
    virtual ~CEnemy() = default;

public:
    virtual HRESULT Initialize_Prototype() = 0;
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta) = 0;
    virtual _int Update(_float fTimeDelta) = 0;
    virtual void Late_Update(_float fTimeDelta) = 0;
    virtual HRESULT Render() = 0;

public:
    virtual void IsPicked(_float3* pOutPos, _float* pDist) override;
    void Plus_HP(_int _iAtt) { m_iHP += _iAtt; }
    int Get_Hp() { return m_iHP; }
    void Reset_Invertia();
    virtual void KillSwitch() override;

protected:
    _float3			m_vInertia      = { 0.f, 0.f, 0.f };
    CFSM*           m_pFSM          = {nullptr};
    CTexture*       m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };
    CTransform*     m_pTransformCom = { nullptr };
    CTransform*     m_pTransformPlayer = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom  = { nullptr };
    CCollider_Cube* m_pColliderCom  = { nullptr };
    CGameObject*    m_pPlayer = { nullptr };
    _float4x4		m_CollisionMatrix;
    _int            m_iMaxHP;
    _int			m_iHP;
    _float          m_fAttackRange;
    _bool           m_bDeadSound = true;

    _float          m_fDeltaPlayer = {(float)INFINITE}; // 거리
    _float3         m_vDeltaPlayer; // 경로벡터

    ACTLEVEL        m_eActLevel;

protected:
    void Friction(_float _FrictionRate); //마찰력 (기본으로 들어가는 감속)
    virtual HRESULT AGGROPlayer() { return S_OK; } //플레이어에게 다가감
    void Compute_DeltaPlayer();
    virtual void Act(_float _fTimeDelta) {};
    void repulsion();
public:
    virtual CGameObject* Clone(void* pArg) override = 0;
    virtual void Free() override = 0;
};

