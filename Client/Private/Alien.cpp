#include "stdafx.h"
#include "Alien.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Player.h"
#include "Effect.h"

CAlien::CAlien(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CEnemy{pGraphic_Device}
{
}

CAlien::CAlien(const CAlien& Prototype)
	:CEnemy{ Prototype }
{
}

HRESULT CAlien::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAlien::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    SetUp_FSM();

    if (FAILED(Ready_Components()))
        return E_FAIL;


    ENEMY_DESC Desc = *(ENEMY_DESC*)pArg;
    m_pTransformCom->Scaling(Desc.fScale.x + 7.f, Desc.fScale.y + 7.f, Desc.fScale.z + 7.f);
    m_pTransformCom->GOTO(Desc.vDir);
    m_iHP = m_iMaxHP = Desc.iMaxHP;
    m_fAttackRange = 10.f;
    m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

    m_vInertia.y = 0.1f;

    m_pFSM->Set_FrameSpeed(CFSM::OBJSTATE_IDLE, 0.f);
    //m_pFSM->Set_FrameSpeed(CFSM::OBJSTATE_ATTACK, 0.f);
    //m_pFSM->Set_FrameSpeed(CFSM::OBJSTATE_DIE, 0.f);

    return S_OK;
}

void CAlien::Priority_Update(_float fTimeDelta)
{
    if (!m_bDead)
        m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);

    Compute_DeltaPlayer();
    AGGROPlayer();
}

int CAlien::Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        //CEffect::EFFECT_DESC effDesc;
        //effDesc.iFrameSpeed = 0.3f;
        //effDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion7");
        //if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &effDesc)))
        //    MSG_BOX(TEXT("NO EFFECT"));

        //m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(Get_Pos());
        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_SM, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlienEffect"));
        return OBJ_DEAD;
    }
    if (Get_Pos().x <= 50 || Get_Pos().x >= 90)
        return OBJ_DEAD;

    Act(fTimeDelta);
    m_pTransformCom->Move(m_vInertia);

    m_vInertia.y -= m_fGravity;


    SetUp_OnTerrain(m_pTransformCom, 3.5f);
    return OBJ_NOEVENT;
}

void CAlien::Late_Update(_float fTimeDelta)
{
    //if (m_iHP <= 0)
    //{
    //    m_pFSM->Set_State(CFSM::OBJSTATE_DIE);
    //}
    //if (OBJ_DEAD == m_pFSM->Late_Update(fTimeDelta))
    //{
    //    m_bDead = true;
    //}
    Friction(m_fFrictionRate);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);


    memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
    m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

    m_pTransformCom->Set_Billboard();
}

HRESULT CAlien::Render()
{
    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    if (FAILED(m_pTextureCom[m_pFSM->Get_CurState()]->Bind_Texture(m_pTextureCom[m_pFSM->Get_CurState()]->Get_Frame().iCurrentTex)))
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

HRESULT CAlien::SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY)
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
        m_pGameInstance->Play3D(TEXT("GreenBeanFace_BoundGround_01"), 3, 1.f, Get_Pos(), false);
        if (CFSM::OBJSTATE_DIE == m_pFSM->Get_CurState()) //사망처리
        {
            vLocalPos.y = fHeight + fOffsetY;

            D3DXVec3TransformCoord(&vWorldPos, &vLocalPos, &WorldMatrix);

            pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);
            return S_OK;
        }

        vLocalPos.y = fHeight + fOffsetY;
        m_vInertia.x = 0.f;
        m_vInertia.y = 0.2f;
        m_vInertia.z = 0.f;

        _float2 vToplayerHorizontal = { m_vDeltaPlayer.x, m_vDeltaPlayer.z };
        D3DXVec2Normalize(&vToplayerHorizontal, &vToplayerHorizontal);
        --m_iAttackCooltime;


        //플레이어 방향으로 점프
		vToplayerHorizontal *= 0.2f;
		m_vInertia.x += vToplayerHorizontal.x;
		m_vInertia.z += vToplayerHorizontal.y; //float2에 저장하는거라서 z를 y에 저장하는거임


        if (m_fDeltaPlayer < 4.f) //플레이어 충돌판정 (공격)
        {
            //_float3 vec = m_pPlayer->Get_Pos() - Get_Pos();
            //D3DXVec3Normalize(&vec, &vec);
            //static_cast<CPlayer*>(m_pPlayer)->Strike_Force( vec * 0.5);
            //static_cast<CPlayer*>(m_pPlayer)->PlusHP(-1);
            m_bDead = true;
        }

    }
    else if (vWorldPos.y > fHeight + fOffsetY + 5.f)
    {
        if (m_iAttackCooltime <= 0) //돌진
        {
            m_pFSM->Set_State(CFSM::OBJSTATE_ATTACK);
            m_iAttackCooltime = 20;
        }
        else
        {
            _float2 vToplayerHorizontal = { m_vDeltaPlayer.x, m_vDeltaPlayer.z };
            D3DXVec2Normalize(&vToplayerHorizontal, &vToplayerHorizontal);
            m_vInertia.x += vToplayerHorizontal.x ;
            m_vInertia.z += vToplayerHorizontal.y * 7;
        }
    }

    /* 지형의 로컬 스페이스 상에 갱신된 로컬위치를 구한것이기때문에 */
    /* 이를 다시 월드로 보내기(지형로컬의 데이터를 -> 월드 )위해서는 월드행렬을 곱해야한다. */
    D3DXVec3TransformCoord(&vWorldPos, &vLocalPos, &WorldMatrix);

    pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);

    return S_OK;
}

HRESULT CAlien::Ready_Components()
{
    /*For Com texture*/
    CTexture::TEXTUREDESC   TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
 //desc 안넣어도 될거같은데?

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RainbowOcto_Move"),
        TEXT("Com_Texture0", &(int)CFSM::OBJSTATE_IDLE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
        return E_FAIL;
    m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);


    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RainbowOcto_Die"),
    //    TEXT("Com_Texture1", &(int)CFSM::OBJSTATE_DIE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_DIE]), &TextureDesc)))
    //    return E_FAIL;
    //m_pFSM->Set_TextureCom(CFSM::OBJSTATE_DIE, m_pTextureCom[CFSM::OBJSTATE_DIE]);

    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RainbowOcto_Attack"),
    //    TEXT("Com_Texture2", &(int)CFSM::OBJSTATE_ATTACK), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_ATTACK]), &TextureDesc)))
    //    return E_FAIL;
    //m_pFSM->Set_TextureCom(CFSM::OBJSTATE_ATTACK, m_pTextureCom[CFSM::OBJSTATE_ATTACK]);

    /* For.Com_VIBuffer */
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

HRESULT CAlien::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

  //  m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    return S_OK;
}

HRESULT CAlien::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


    return S_OK;
}

HRESULT CAlien::SetUp_FSM()
{
    m_pFSM = CFSM::Create(nullptr);

    CFSM::Frame_DESC Desc;
    Desc = { 27, 1 };
    m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);
    Desc = { 51, 1 };
    m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_DIE, Desc);
    Desc = { 41, 1 };
    m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_ATTACK, Desc);

    m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
    return S_OK;
}

HRESULT CAlien::AGGROPlayer()
{
    if (m_fAttackRange < 0)
    {
        MSG_BOX(TEXT("플레이어 인식 거리 오류!"));
        return E_FAIL;
    }

    if (m_fDeltaPlayer <= m_fAttackRange)
    {
        m_eActLevel = ACT_ATTACK;
    }
    else
    {
        m_eActLevel = ACT_AGGRO;
    }

    return S_OK;
}

void CAlien::Act(_float _fTimeDelta) {    }



CAlien* CAlien::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CAlien* pInstance = new CAlien(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAlien"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAlien::Clone(void* pArg)
{
    CAlien* pInstance = new CAlien(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CAlien"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAlien::Free()
{
    __super::Free();


}
