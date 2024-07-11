#include "stdafx.h"
#include "..\Public\WormCorn.h"

#include "GameInstance.h"
#include "Player.h"

CWormCorn::CWormCorn(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CWormCorn::CWormCorn(const CWormCorn& Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CWormCorn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWormCorn::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CWormCorn::Priority_Update(_float fTimeDelta)
{
}

int CWormCorn::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
	m_pTransformCom->Go_Up(fTimeDelta);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_WALL, this);
	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);
	return OBJ_NOEVENT;
}

void CWormCorn::Late_Update(_float fTimeDelta)
{
	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		/*CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		CTransform* pTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
		_float3 vPosition = pTransform->Get_State(CTransform::STATE_POSITION);

		if (fabsf(vDistance.x) < fabsf(vDistance.z))
			vPosition.x += vDistance.x;
		else
			vPosition.z += vDistance.z;

		pTransform->Set_State(CTransform::STATE_POSITION, vPosition);*/
	}

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 0.7f)
		m_bDead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CWormCorn::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CWormCorn::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapCylinder"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Corn"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 2.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

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

CWormCorn* CWormCorn::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWormCorn* pInstance = new CWormCorn(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWormCorn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CWormCorn::Clone(void* pArg)
{
	CWormCorn* pInstance = new CWormCorn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CWormCorn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWormCorn::Free()
{
	__super::Free();


	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
