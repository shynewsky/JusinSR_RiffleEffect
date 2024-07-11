#include "stdafx.h"
#include "..\Public\Rayzer.h"

#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Bullet.h"

CRayzer::CRayzer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CRayzer::CRayzer(const CRayzer & Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CRayzer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRayzer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	Ready_VIBuffer();

	if (!pArg)
	{
		m_vStartPos = _float3(50.f, 3.f, 50.f);
		return S_OK;
	}
	RAYZER_DESC* pDesc = static_cast<RAYZER_DESC*>(pArg);

	m_vStartPos = pDesc->vStartPos;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CRayzer::Priority_Update(_float fTimeDelta)
{
}

int CRayzer::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;


	Update_VIBuffer();
	m_fSumTime += fTimeDelta;

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);

	return OBJ_NOEVENT;
}

void CRayzer::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);


	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{

	}
}

HRESULT CRayzer::Render()
{	

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	Render_VIBuffer();

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}


HRESULT CRayzer::Ready_Components()
{
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

HRESULT CRayzer::Ready_VIBuffer()
{
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(2 * sizeof(VERTEXCOL), 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &m_pVBuffer, NULL)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRayzer::Update_VIBuffer()
{
	VERTEXCOL* pVertices = nullptr;
	
	LEVELID eLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
	CGameObject* pPlayer = m_pGameInstance->Find_Object(eLevel, TEXT("Layer_Player"));
	if (!pPlayer)
		S_OK;

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
	pVertices[0].vPosition = m_vStartPos;
	pVertices[0].color = D3DCOLOR_ARGB(255,255, 255, 255);
	pVertices[1].vPosition = pPlayer->Get_Pos();
	pVertices[1].color = D3DCOLOR_ARGB(255, 255, 255, 255);
	
	m_pVBuffer->Unlock();
	return S_OK;
}

HRESULT CRayzer::Render_VIBuffer()
{
	m_pGraphic_Device->SetStreamSource(0, m_pVBuffer, 0, sizeof(VERTEXCOL));
	m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	m_pGraphic_Device->DrawPrimitive(D3DPT_LINESTRIP, 0, 1);
	

	return S_OK;
}

HRESULT CRayzer::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE, 50.0f);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	return S_OK;
	
}

HRESULT CRayzer::Reset_RenderState()
{
	/*m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);*/
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE); // 조명 활성화
	return S_OK;
}


CRayzer * CRayzer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRayzer*		pInstance = new CRayzer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRayzer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CRayzer::Clone(void* pArg)
{
	CRayzer*		pInstance = new CRayzer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CRayzer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRayzer::Free()
{
	__super::Free();

	Safe_Release(m_pVBuffer);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
