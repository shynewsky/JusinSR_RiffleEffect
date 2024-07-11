#include "stdafx.h"
#include "Grenade.h"
#include "Enemy.h"
#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Effect.h"


CGrenade::CGrenade(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{ pGraphic_Device }
{
}

CGrenade::CGrenade(const CGrenade& Prototype)
	:CLandObject{ Prototype }
{
}

HRESULT CGrenade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGrenade::Initialize(void* pArg)
{

	GRENADE_DESC* pDesc = static_cast<GRENADE_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
#pragma region Desc

	m_vLook = pDesc->vLook;
	m_pTerrainVIBuffer = pDesc->pTerrainVIBuffer;
	Safe_AddRef(m_pTerrainVIBuffer);
	m_pTerrainTransform = pDesc->pTerrainTransform;
	Safe_AddRef(m_pTerrainTransform);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_vInitialLook = pDesc->vPos - pDesc->vLook;
	m_eCurLevel = pDesc->eCurLevel;
#pragma endregion

	m_vInertia += _float3{0.f, pDesc->fPower, 0.f} + m_vLook * pDesc->fPower;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pTransformCom->Scaling(1.f, 1.f, 1.f);
	//D3DXMatrixIdentity(&m_CollisionMatrix);
	return S_OK;
}

void CGrenade::Priority_Update(_float fTimeDelta)
{
}

int CGrenade::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
		list<class CGameObject*>* listObj = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_Monster"));
		if (nullptr == listObj)
			return OBJ_DEAD;
		for (auto iter = listObj->begin(); iter != listObj->end(); ++iter)
		{
			if (*iter == nullptr)
				continue;
			if (D3DXVec3Length(&((*iter)->Get_Pos() - Get_Pos())) < 3.f)
			{
				(*iter)->KillSwitch(); //즉사코드가 아니라 죽을때 주면에 데미지 주면 될듯
			}
			CEffect::EFFECT_DESC effDesc;
			effDesc.iFrameSpeed = 0.5f;
			effDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion1");

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &effDesc)))
				MSG_BOX(TEXT("NO EFFECT"));
			m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(Get_Pos());
			static_cast<CTransform*>(m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Find_Component(TEXT("Com_Transform")))->Scaling(6.f, 6.f, 1.f);
			m_pGameInstance->PlayDefault(TEXT("Grenade.wav"), 14, 0.6f, false);
		}
		return OBJ_DEAD;
	}

	m_pTransformCom->Move(m_vInertia * fTimeDelta);
	m_vInertia += _float3{ 0.f, -5.f, 0.f } *fTimeDelta * 2;

	//memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	//m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	//m_fCollisionTime += fTimeDelta;
	//m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);

	return OBJ_NOEVENT;
}

void CGrenade::Late_Update(_float fTimeDelta)
{

	if (SetUp_UnderTerrain(m_pTransformCom, 0.5f))
		m_bDead = true;


	_float3 vDistance = _float3(0, 0, 0);
	if (m_fCollisionTime >= 0.1f)
		if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_MONSTER, this, CCollider_Manager::COLLSIION_BOX, &vDistance)
			|| m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
		{

		}


	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < -1.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 50.f)
		m_bDead = true;

	m_pTextureCom->Move_Frame(fTimeDelta, true);
	m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CGrenade::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrenade::Ready_Components()
{
	//텍스쳐
	CTexture::TEXTUREDESC   TextureDesc = {0, 0, 4, 0.1f};
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grenade"),
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

HRESULT CGrenade::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
}

HRESULT CGrenade::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CGrenade* CGrenade::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGrenade* pInstance = new CGrenade(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : Grenade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrenade::Clone(void* pArg)
{
	CGrenade* pInstance = new CGrenade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : Grenade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrenade::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}
