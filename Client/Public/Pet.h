#pragma once
#include "LandObject.h"

#include "Player.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

BEGIN(Client)

class CPet final : public CLandObject
{
public:
    enum PET_STATE { PET_NORMAL, PET_HEAL, PET_BUFF, PET_FOLLOW, PET_END };
    enum PET_SIDE { PET_RIGHT, PET_LEFT, PET_NONE };

private:
    CPet(LPDIRECT3DDEVICE9 pGraphic_Device);
    CPet(const CPet& Prototype);
    virtual ~CPet() = default;

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
    void    Set_BillBoard();

private:
    void Fly_Route();
    void Set_PetState();
    void PetState_Action();

private:
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CTransform* m_pTransformPlayer = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CCollider_Cube* m_pColliderCom = { nullptr };
    CPlayer* m_pPlayer = { nullptr };

    PET_STATE m_ePetState = PET_END;
    PET_SIDE m_ePetSide = PET_NONE;

    _float	m_fFrame = {};
    _float4x4 m_CollisionMatrix;

private:
    _float3 m_vPlayerPos = {};
    _float3 m_vPetPos = {};
    _float3 m_vPlayerToPet = {};

    _float3 m_vPlayerLook = {};
    _float m_fUnderLine = {};

    _float3 m_vPlayerRIght = {};
    _float m_fHeightLine = {};

private:
    bool m_bHpTimeStart = false;
    bool m_bFeverTimeStart = false;
    bool m_bCanChangeState = false;

    DWORD m_dwHpTime = {};
    DWORD m_dwFeverTime = {};
    DWORD m_dwStateTime = {};

public:
    static CPet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END