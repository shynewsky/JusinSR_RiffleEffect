#include "stdafx.h"
#include "Bullet_Arc.h"

#include "GameInstance.h"
#include "Effect.h"


CBullet_Arc::CBullet_Arc(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{ pGraphic_Device }
{
}

CBullet_Arc::CBullet_Arc(const CBullet_Arc& Prototype)
	:CLandObject{ Prototype }
{
}

HRESULT CBullet_Arc::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet_Arc::Initialize(void* pArg)
{

	BULLETARC_DESC* pDesc = static_cast<BULLETARC_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

#pragma region Desc

	m_vTargetPos = pDesc->vTargetPos;
	m_pTerrainVIBuffer = pDesc->pTerrainVIBuffer;
	Safe_AddRef(m_pTerrainVIBuffer);
	m_pTerrainTransform = pDesc->pTerrainTransform;
	Safe_AddRef(m_pTerrainTransform);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_vInitialLook = pDesc->vPos - pDesc->vTargetPos;

#pragma endregion

	m_vInertia += {0.f, 5.f, 0.f};
	
	m_vInertia += m_vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	//m_pTransformCom->Scaling(0.2f, 0.2f, 1.f);
	//D3DXMatrixIdentity(&m_CollisionMatrix);
	return S_OK;
}

void CBullet_Arc::Priority_Update(_float fTimeDelta)
{

}

int CBullet_Arc::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;
	
	m_pTransformCom->Move(m_vInertia * fTimeDelta);
	m_vInertia += _float3{ 0.f, -5.f, 0.f } * fTimeDelta * 2;
	m_pTextureCom->Move_Frame(fTimeDelta);
	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	m_fCollisionTime += fTimeDelta;
	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);

	return OBJ_NOEVENT;
}

void CBullet_Arc::Late_Update(_float fTimeDelta)
{

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < 0.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 50.f)
		m_bDead = true;
	_float3 vDistance;
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
		m_bDead = true;

	if (m_bDead)
	{
		CEffect::EFFECT_DESC Desc;
		Desc.iFrameSpeed = 0.3f;
		Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_RedBulletEffect");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Desc)))
			MSG_BOX(TEXT("NO EFFECT"));

		m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(__super::Get_Pos());
	}

	
	
	m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBullet_Arc::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
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

	//m_pColliderCom->Render_ColliderBox();

	return S_OK;
}

HRESULT CBullet_Arc::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC TextureDesc;
	TextureDesc.fSpeed = 0.02f;
	TextureDesc.iCurrentTex = 0;
	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 4;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RedBullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &TextureDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 5.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube*/

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 0.1f;
	CollCubeDesc.fRadiusX = 0.1f;
	CollCubeDesc.fRadiusZ = 0.1f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Arc::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
}

HRESULT CBullet_Arc::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CBullet_Arc* CBullet_Arc::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBullet_Arc* pInstance = new CBullet_Arc(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBullet_Arc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBullet_Arc::Clone(void* pArg)
{
	CBullet_Arc* pInstance = new CBullet_Arc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBullet_Arc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet_Arc::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}
