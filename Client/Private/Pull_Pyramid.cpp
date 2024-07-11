#include "stdafx.h"
#include "Pull_Pyramid.h"

#include "GameInstance.h"
#include "ParticleSystem.h"

CPull_Pyramid::CPull_Pyramid(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CPull_Pyramid::CPull_Pyramid(const CBlendObject& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CPull_Pyramid::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPull_Pyramid::Initialize(void* pArg)
{
	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vNeroPos = pDesc->vStartPos;
	m_vPlayerPos = pDesc->vEndPos;
	m_pTransformPlayer = pDesc->TransformPlayer;
	//Safe_AddRef(m_pTransformPlayer);

#pragma region 피라미드 기울이기

	// 바닥에서 보스으로의 A벡터 구하기
	// 보스 = startpos.x , startpos.y, startpos.z
	// 바닥 = startpos.x , 0.5f , startpos.z
	_float3 vFloorToBoss = {
		0.f,
		pDesc->vStartPos.y - 0.5f,
		0.f
	};

	// 플레이어에서 바닥까지의 B벡터 구하기
	// 바닥 = startpos.x , 0.5f , startpos.z
	// 플레이어 = endpos.x , 0.5f, endpos.z
	_float3 vPlayerToFloor = {
		pDesc->vStartPos.x - pDesc->vEndPos.x,
		0.f,
		pDesc->vStartPos.z - pDesc->vEndPos.z
	};

	// B벡터 + A벡터 = UP 벡터
	_float3 vUpPyramid = vPlayerToFloor + vFloorToBoss;

	// B벡터 X A벡터 = RIGHT 벡터
	_float3 vRightPyramid = {};
	D3DXVec3Cross(&vRightPyramid, &vPlayerToFloor, &vFloorToBoss);

	// RIGHT벡터 X UP벡터 = LOOK 벡터
	_float3 vLookPyramid = {};
	D3DXVec3Cross(&vLookPyramid, &vRightPyramid, &vUpPyramid);

	// (End - Start) / 2 = POSITION 벡터
	_float3 vPositionPyramid = (pDesc->vEndPos + pDesc->vStartPos) / 2.f;

	D3DXVec3Normalize(&vUpPyramid, &vUpPyramid);
	D3DXVec3Normalize(&vLookPyramid, &vLookPyramid);
	D3DXVec3Normalize(&vRightPyramid, &vRightPyramid);

	m_pTransformCom->Set_State(CTransform::STATE_UP, vUpPyramid);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLookPyramid);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRightPyramid);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPositionPyramid);

#pragma endregion

#pragma region 피라미드 크기설정

	_float3 vLengthPyramid = pDesc->vEndPos - pDesc->vStartPos;
	_float fLengthPyramid = D3DXVec3Length(&vLengthPyramid);

	m_pTransformCom->Scaling(5.f, fLengthPyramid, 5.f);

#pragma endregion

#pragma region 피라미드 충돌범위

	D3DXVec3Normalize(&m_vSideNormal, &vPlayerToFloor); // 축 단위벡터 (보스바닥에서 플레이어까지)
	D3DXVec3Normalize(&m_vFarNormal, &vRightPyramid); // 축 단위벡터 (보스바닥에서 플레이어까지)

	m_fPyramidCenter = vPositionPyramid; // 피라미드 중심점
	m_fPyramidHeight = D3DXVec3Length(&(m_vPlayerPos - m_fPyramidCenter)); // 피라미드 높이

#pragma endregion

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_vMyLook = { 0.f, 0.f, 0.f };

	m_dwTime = GetTickCount();

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Play3D(TEXT("Nero_PullBoom.wav"), 21, 1.f, vPos, false);
	m_pGameInstance->Play3D(TEXT("Nero_Pull.wav"), 21, 1.f, vPos, true);

#pragma endregion

	return S_OK;
}

void CPull_Pyramid::Priority_Update(_float fTimeDelta)
{
}

int CPull_Pyramid::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
#pragma region 테스트코드

		m_pGameInstance->StopSound(21);

#pragma endregion

		return OBJ_DEAD;
	}

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return OBJ_NOEVENT;

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Set_SoundPos(21, vPos);

#pragma endregion

#pragma region 플레이어 당기기

	// 옆으로 이동한 범위 확인하기
	_float3 vMovedFromPrePos = m_pTransformPlayer->Get_State(CTransform::STATE_POSITION) - m_vPlayerPos; 	// 플레이어 기존위치에서 현위치까지의 벡터

	_float fRedSlope = D3DXVec3Length(&vMovedFromPrePos); // 빗변길이
	_float fRedWidth = abs(D3DXVec3Dot(&m_vSideNormal, &vMovedFromPrePos)); // 정사영 길이 (밑변길이)
	_float fRedHeight = sqrt((fRedSlope * fRedSlope) - (fRedWidth * fRedWidth)); // 높이길이

	// 멀리 이동한 범위 확인하기
	_float3 vMovedFromCenter = m_pTransformPlayer->Get_State(CTransform::STATE_POSITION) - m_fPyramidCenter; // 피라미드 중심에서 플레이어 현위치까지의 벡터

	_float fBlueSlope = D3DXVec3Length(&vMovedFromCenter); // 빗변길이
	_float fBlueWidth = D3DXVec3Dot(&m_vFarNormal, &vMovedFromCenter); // 정사영 길이 (밑변길이)
	_float fBlueHeight = sqrt((fBlueSlope * fBlueSlope) - (fBlueWidth * fBlueWidth)); // 높이길이

	if (fRedHeight < 2.f
		&& fBlueHeight < m_fPyramidHeight)
	{
		// 당기는 방향 (바닥 - 플레이어)
		// 바닥 : (m_vNeroPos.x, 0.5f, m_vNeroPos.z)
		// 플레이어 : (m_vPlayerPos.x, 0.5f, m_vPlayerPos.z)
		_float3 vPullDir = { m_vNeroPos.x - m_vPlayerPos.x, 0.f, m_vNeroPos.z - m_vPlayerPos.z };
		D3DXVec3Normalize(&vPullDir, &vPullDir);

		// 이동
		// 위치 += 방향벡터
		// 위치 = 위치 + 방향벡터
		m_pTransformPlayer->Set_State(CTransform::STATE_POSITION,
			m_pTransformPlayer->Get_State(CTransform::STATE_POSITION) + vPullDir);
		//m_pTransformPlayer->Move(vPullDir);
	}

#pragma endregion

#pragma region 사각뿔 삭제

	if (m_dwTime + 3000 < GetTickCount())
	{
		m_bDead = true;
	}

#pragma endregion

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	m_fCollisionTime += fTimeDelta;

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);
	return OBJ_NOEVENT;
}

void CPull_Pyramid::Late_Update(_float fTimeDelta)
{
	if (m_fCollisionTime <= 0.1f)
		return;

	_float3 vDistance = _float3(0, 0, 0);

	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		m_bDead = true;
	}
	//if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	//{
	//	m_bDead = true;
	//}
	else if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < -1.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 50.f)
		m_bDead = true;

	//m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CPull_Pyramid::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	//m_pColliderCom->Render_ColliderBox();

	return S_OK;
}

HRESULT CPull_Pyramid::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Pull_Pyramid"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Pyramid"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC TransformDesc{};

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube*/

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 0.3f;
	CollCubeDesc.fRadiusX = 0.3f;
	CollCubeDesc.fRadiusZ = 0.3f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPull_Pyramid::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);*/

	return S_OK;
}

HRESULT CPull_Pyramid::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPull_Pyramid* CPull_Pyramid::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPull_Pyramid* pInstance = new CPull_Pyramid(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPull_Pyramid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPull_Pyramid::Clone(void* pArg)
{
	CPull_Pyramid* pInstance = new CPull_Pyramid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CPull_Pyramid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPull_Pyramid::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
