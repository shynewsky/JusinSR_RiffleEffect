#pragma once
#include "LandObject.h"
#include "FSM.h"


BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END


class CEnemy_SM :
    public CLandObject
{
public:
    typedef struct EnemySM_Desc
    {
        CVIBuffer_Terrain* pTerrainVIBuffer = { nullptr };
        CTransform* pTerrainTransform = { nullptr };
        _float3 fScale = { 1.f, 1.f, 1.f };
        _float3 vPos;
        _bool Left = true; //진행방향
    };
protected:
    CEnemy_SM(LPDIRECT3DDEVICE9 pGraphic_Device);
    CEnemy_SM(const CEnemy_SM& Prototype);
    virtual ~CEnemy_SM() = default;

public:
    virtual HRESULT Initialize_Prototype() = 0;
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta) = 0;
    virtual _int Update(_float fTimeDelta) = 0;
    virtual void Late_Update(_float fTimeDelta) = 0;
    virtual HRESULT Render() = 0;

protected:
    CFSM* m_pFSM;
    _bool m_bLeft;
    CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = {nullptr};
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CCollider_Cube* m_pColliderCom = { nullptr };
    _float4x4		m_CollisionMatrix;


public: //추상클래스라 create 없는거임
    virtual CGameObject* Clone(void* pArg) override = 0;
    virtual void Free() override;
};