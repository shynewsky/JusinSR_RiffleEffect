#include "stdafx.h"
#include "Beam_Cylinder.h"

#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Player.h"
#include "Effect.h"

CBeam_Cylinder::CBeam_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{ pGraphic_Device }
{
}

CBeam_Cylinder::CBeam_Cylinder(const CBlendObject& Prototype)
	:CBlendObject{ Prototype }
{
}

HRESULT CBeam_Cylinder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeam_Cylinder::Initialize(void* pArg)
{
	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

#pragma region 기울이기

	// 보스에서 바닥으로의 A벡터 구하기
	// 바닥 = startpos.x , 0.5f , startpos.z
	// 보스 = startpos.x , startpos.y, startpos.z
	_float3 vBossToFloor = {
		0.f,
		0.5f - pDesc->vStartPos.y,
		0.f
	};

	// 바닥에서 플레이어까지의 B벡터 구하기
	// 플레이어 = endpos.x , 0.5f, endpos.z
	// 바닥 = startpos.x , 0.5f , startpos.z
	_float3 vFloorToPlayer = {
		pDesc->vEndPos.x - pDesc->vStartPos.x ,
		0.f,
		pDesc->vEndPos.z - pDesc->vStartPos.z
	};

	// A벡터 + B벡터 = UP 벡터
	_float3 vUpCylinder = vBossToFloor + vFloorToPlayer;

	// B벡터 X A벡터 = LOOK 벡터
	_float3 vLookCylinder = {};
	D3DXVec3Cross(&vLookCylinder, &vFloorToPlayer, &vBossToFloor);

	// UP벡터 X LOOK벡터 = RIGHT 벡터
	_float3 vRIghtCylinder = {};
	D3DXVec3Cross(&vRIghtCylinder, &vUpCylinder, &vLookCylinder);

	// (End - Start) / 2 = POSITION 벡터
	_float3 vPositionCylinder = (pDesc->vEndPos + pDesc->vStartPos) / 2.f;

	D3DXVec3Normalize(&vUpCylinder, &vUpCylinder);
	D3DXVec3Normalize(&vLookCylinder, &vLookCylinder);
	D3DXVec3Normalize(&vRIghtCylinder, &vRIghtCylinder);

	m_pTransformCom->Set_State(CTransform::STATE_UP, vUpCylinder);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLookCylinder);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRIghtCylinder);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPositionCylinder);

#pragma endregion

	_float3 vLengthCylinder = pDesc->vEndPos - pDesc->vStartPos;
	_float fLengthCylinder = D3DXVec3Length(&vLengthCylinder);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_vMyLook = { 0.f, 0.f, 0.f };

	m_pTransformCom->Scaling(5.f, fLengthCylinder * 2.f, 5.f);

	m_dwTime = GetTickCount();

#pragma region 충돌계산용 변수 초기화
	m_vStart = pDesc->vStartPos;
	m_vEnd = pDesc->vEndPos;
	m_pPlayer = m_pGameInstance->Get_ObjectList((LEVEL_NERO), TEXT("Layer_Player"))->front();

#pragma endregion

	CEffect::EFFECT_DESC Desc;
	Desc.iFrameSpeed = 1.f;
	Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion2");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_No_Billboard"), &Desc)))
		MSG_BOX(TEXT("NO EFFECT"));

	CGameObject* peffect = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back();
	peffect->Set_Pos(m_vEnd);
	static_cast<CEffect*>(peffect)->Scaling(30.f, 30.f, 3.f);
	static_cast<CEffect*>(peffect)->Lie_On();

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Play3D(TEXT("Nero_BeamBoom.wav"), 19, 1.f, vPos, false);
	m_pGameInstance->Play3D(TEXT("Nero_BeamBoom.wav"), 19, 1.f, vPos, false);
	m_pGameInstance->Play3D(TEXT("Nero_BeamBoom.wav"), 19, 1.f, vPos, false);
	m_pGameInstance->Play3D(TEXT("Nero_Beam.wav"), 19, 1.f, vPos, true);

#pragma endregion

	return S_OK;
}

void CBeam_Cylinder::Priority_Update(_float fTimeDelta)
{
}

int CBeam_Cylinder::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
#pragma region 테스트코드

		m_pGameInstance->StopSound(19);

#pragma endregion

		return OBJ_DEAD;
	}

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return OBJ_NOEVENT;

#pragma region 테스트코드

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Set_SoundPos(19, vPos);

#pragma endregion

#pragma region 원기둥 프레임 돌리기

	m_fFrame += 20 * fTimeDelta; // 1초에 20장 넘기고

	if (m_fFrame >= 5.f) // 5초 = 100장
		m_fFrame = 0.f;

#pragma endregion

#pragma region 원기둥 삭제

	if (m_dwTime + 3000 < GetTickCount())
	{
		m_bDead = true;
	}

#pragma endregion

	//memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	//m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	m_fCollisionTime += fTimeDelta;
	m_fSumTime += fTimeDelta;
	//m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);


	if (m_fSumTime >= 0.2f)
	{
		m_fSumTime = 0.f;
		CEffect::EFFECT_DESC Desc;
		Desc.iFrameSpeed = 1.f;
		Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion2");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_No_Billboard"), &Desc)))
			MSG_BOX(TEXT("NO EFFECT"));

		CGameObject* peffect = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back();
		m_vEnd.y += 0.0001;
		peffect->Set_Pos(m_vEnd);
		static_cast<CEffect*>(peffect)->Scaling(30.f, 30.f, 3.f);
		static_cast<CEffect*>(peffect)->Lie_On();
	}
	

	return OBJ_NOEVENT;
}

void CBeam_Cylinder::Late_Update(_float fTimeDelta)
{
	if (m_fCollisionTime <= 0.1f)
		return;

	_float3 vDistance = _float3(0, 0, 0);

	//if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	//{
	//	m_bDead = true;
	//}
	//if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	//{
	//	m_bDead = true;
	//}
	//else if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < -1.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > 50.f)
	//	m_bDead = true;

	//m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
	Check_DeltaPlayer(); //충돌판정 + 후처리
}

HRESULT CBeam_Cylinder::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame)))
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

HRESULT CBeam_Cylinder::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Beam_Cylinder"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cylinder"),
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

HRESULT CBeam_Cylinder::SetUp_RenderState()
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

HRESULT CBeam_Cylinder::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

_bool CBeam_Cylinder::Check_DeltaPlayer()
{
	_float fUnder = D3DXVec3Length(&(m_vEnd - m_pPlayer->Get_Pos()));
	_float3 vUpper;
	D3DXVec3Normalize(&vUpper, &(m_vEnd - m_vStart));

	_float fUpper = abs(D3DXVec3Dot(&(m_vEnd - m_pPlayer->Get_Pos()), &vUpper));
	
	_float delta = sqrt(pow(2, fUnder) - pow(2, fUpper));
	if (delta < 3.f) //원기둥 두께 얼마인지 몰라서 3으로 임시처리
	{
		static_cast<CPlayer*>(m_pPlayer)->HitPlayer(1);
	}
	return false;
}

void CBeam_Cylinder::GronudEffect() // 디테일하게 이런거 하기 전에 일단 간단하게 구현 해놔야될듯
{
	/*_float3 fDPlayer = { m_vEnd - m_vStart };
	D3DXVec3Normalize(&fDPlayer, fDPlayer);
	_float ftheta = D3DXVec3Dot(&fDPlayer, &_float3{ 0.f, fDPlayer.y , 0.f });*/
}

CBeam_Cylinder* CBeam_Cylinder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBeam_Cylinder* pInstance = new CBeam_Cylinder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBeam_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBeam_Cylinder::Clone(void* pArg)
{
	CBeam_Cylinder* pInstance = new CBeam_Cylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBeam_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeam_Cylinder::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
