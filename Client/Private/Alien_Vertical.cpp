#include "stdafx.h"
#include "Alien_Vertical.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Effect.h"

CAlien_Vertical::CAlien_Vertical(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CEnemy{pGraphic_Device}
{
}

CAlien_Vertical::CAlien_Vertical(const CAlien_Vertical& Prototype)
	:CEnemy{ Prototype }
{
}

HRESULT CAlien_Vertical::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAlien_Vertical::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    ENEMY_DESC Desc = *(ENEMY_DESC*)pArg;
    m_pTransformCom->Scaling(Desc.fScale.x + 7.f, Desc.fScale.y + 7.f, Desc.fScale.z + 7.f);
    m_pTransformCom->GOTO(Desc.vDir);
    m_iHP = m_iMaxHP = Desc.iMaxHP;
    m_fAttackRange = 10.f;
    m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

    m_vInertia.y = 0.1f;

    return S_OK;
}

void CAlien_Vertical::Priority_Update(_float fTimeDelta)
{
    if (!m_bDead)
        m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);

    Compute_DeltaPlayer();
}

int CAlien_Vertical::Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlienEffect"));
        return OBJ_DEAD;
    }
    _float3 vPos = Get_Pos();
    if (vPos.x <= 50 || vPos.x >= 90 || vPos.z <= 0)
        return OBJ_DEAD;

    Act(fTimeDelta);
    m_pTransformCom->Move(m_vInertia);
    m_pTransformCom->Move(_float3{ 0.f, 0.f, 3 * -fTimeDelta });
    m_vInertia.y -= m_fGravity;


    SetUp_OnTerrain(m_pTransformCom, 3.5f);
    return OBJ_NOEVENT;
}

void CAlien_Vertical::Late_Update(_float fTimeDelta)
{
    Friction(m_fFrictionRate);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);


    memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
    m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

    m_pTransformCom->Set_Billboard();
}

HRESULT CAlien_Vertical::Render()
{
    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    if (FAILED(m_pTextureCom[0]->Bind_Texture(m_pTextureCom[0]->Get_Frame().iCurrentTex)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    _float4x4		ViewMatrix{};
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

    if (FAILED(SetUp_RenderState()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Reset_RenderState()))
        return E_FAIL;

    return S_OK;
}

HRESULT CAlien_Vertical::SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY)
{

    _float3		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION); //객체의 월드 Pos

    _float4x4		WorldMatrixInverse = m_pTerrainTransform->Get_WorldMatrix_Inverse(); //지형의 월드 역행렬
    _float4x4		WorldMatrix = m_pTerrainTransform->Get_WorldMatrix(); // 지형의 월드행렬

    _float3		vLocalPos{};
    D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, &WorldMatrixInverse);

    _float		fHeight = m_pTerrainVIBuffer->Compute_Height(vLocalPos);

    //지형이 객체보다 높으면 올려줌
    if (vWorldPos.y < fHeight + fOffsetY)
    {

        vLocalPos.y = fHeight + fOffsetY;
        m_vInertia.x = 0.f;
        m_vInertia.y = 0.2f;
        m_vInertia.z = 0.f;

        _float2 vToplayerHorizontal = { m_vDeltaPlayer.x, m_vDeltaPlayer.z };
        D3DXVec2Normalize(&vToplayerHorizontal, &vToplayerHorizontal);
        --m_iAttackCooltime;

        if (m_fDeltaPlayer < 4.f) //플레이어 충돌판정 (공격)
        {
            m_bDead = true;
        }

    }
    else if (vWorldPos.y > fHeight + fOffsetY + 5.f)
    {
            _float2 vToplayerHorizontal = { m_vDeltaPlayer.x, m_vDeltaPlayer.z };
            D3DXVec2Normalize(&vToplayerHorizontal, &vToplayerHorizontal);
            m_vInertia.x += vToplayerHorizontal.x ;
            m_vInertia.z += vToplayerHorizontal.y * 7;
    }

    D3DXVec3TransformCoord(&vWorldPos, &vLocalPos, &WorldMatrix);

    pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);

    return S_OK;
}

HRESULT CAlien_Vertical::Ready_Components()
{
    /*For Com texture*/
    CTexture::TEXTUREDESC   TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
 //desc 안넣어도 될거같은데?

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RainbowOcto_Move"),
        TEXT("Com_Texture0", &(int)CFSM::OBJSTATE_IDLE), reinterpret_cast<CComponent**>(&m_pTextureCom[0]), &TextureDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;
 
    /*For Com Transform*/
    CTransform::TRANSFORM_DESC      TransformDesc{};

    TransformDesc.fSpeedPerSec = 10.f;
    TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    m_pPlayer = m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Player"));
    Safe_AddRef(m_pPlayer);
    m_pTransformPlayer = static_cast<CTransform*>(m_pGameInstance->Find_Component(m_eCurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
    Safe_AddRef(m_pTransformPlayer);

    /* For.Com_Collider_Cube*/

    CCollider_Cube::COLLIDER_DESC CollCubeDesc;
    ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
    CollCubeDesc.fRadiusY = 0.5f;
    CollCubeDesc.fRadiusX = 0.5f;
    CollCubeDesc.fRadiusZ = 0.5f;
    CollCubeDesc.fOffSetX = 0.f;
    CollCubeDesc.fOffSetY = 0.f;
    CollCubeDesc.fOffsetZ = 0.f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
        TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
        return E_FAIL;

    return S_OK;

}

HRESULT CAlien_Vertical::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

    return S_OK;
}

HRESULT CAlien_Vertical::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


    return S_OK;
}

CAlien_Vertical* CAlien_Vertical::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CAlien_Vertical* pInstance = new CAlien_Vertical(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAlien_Vertical"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAlien_Vertical::Clone(void* pArg)
{
    CAlien_Vertical* pInstance = new CAlien_Vertical(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CAlien_Vertical"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAlien_Vertical::Free()
{
    __super::Free();


}
