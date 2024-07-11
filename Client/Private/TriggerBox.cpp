#include "stdafx.h"
#include "..\Public\TriggerBox.h"

#include "GameInstance.h"
#include "Player.h"
#include "Camera.h"
#include "ScreenEffect.h"

CTriggerBox::CTriggerBox(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CTriggerBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTriggerBox::Initialize(void* pArg)
{
	TRIGGER_INFO* pDesc = static_cast<TRIGGER_INFO*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eType = pDesc->eType;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->pos);
	m_iCount = pDesc->iCount;
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CTriggerBox::Priority_Update(_float fTimeDelta)
{

}

int CTriggerBox::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	//m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_WALL, this);

	return OBJ_NOEVENT;
}

void CTriggerBox::Late_Update(_float fTimeDelta)
{
	if (m_bDead)
		return;

	_float3 vDistance = _float3(0, 0, 0);
	if (m_bFirst == false && m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		m_bEvent = true;
		m_bFirst = true;
	}
	else
		m_bEvent = false;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTriggerBox::Render()
{


	return S_OK;
}

HRESULT CTriggerBox::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube*/

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 1.f;
	CollCubeDesc.fRadiusX = 4.f;
	CollCubeDesc.fRadiusZ = 3.f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}



CTriggerBox* CTriggerBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTriggerBox* pInstance = new CTriggerBox(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTriggerBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CTriggerBox::Clone(void* pArg)
{
	CTriggerBox* pInstance = new CTriggerBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTriggerBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriggerBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
