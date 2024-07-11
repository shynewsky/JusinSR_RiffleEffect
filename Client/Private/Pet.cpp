#include "stdafx.h"
#include "Pet.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Player.h"
#include "Effect.h"
#include <iostream>

#include "ScreenEffect.h"

CPet::CPet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CLandObject{ pGraphic_Device }
{
}

CPet::CPet(const CPet& Prototype)
	:CLandObject{ Prototype }
{
}

HRESULT CPet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPet::Initialize(void* pArg)
{
	if (__super::Initialize(pArg)) // LandObject 상속을 통해서 터레인 정보를 가져온다
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float3 vPlayerPos = m_pTransformPlayer->Get_State(CTransform::STATE_POSITION);
	_float3 vPlayerLook = m_pTransformPlayer->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos + (vPlayerLook * -1.f));
	m_pTransformCom->Scaling(0.5f, 0.5f, 0.5f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix(); // 충돌체에 내 월드행렬을 부여

#pragma region 테스트코드
	m_ePetState = PET_NORMAL;
	m_ePetSide = PET_RIGHT;
#pragma endregion

	return S_OK;
}

void CPet::Priority_Update(_float fTimeDelta)
{
}

int CPet::Update(_float fTimeDelta)
{
	Fly_Route();

	Set_PetState();

	PetState_Action();

	m_fFrame += fTimeDelta * 12.f; // 1초에 12장 돌리는 속도

	if (m_fFrame >= 4.f) // 4번때 이미지 오면 다시 루프
		m_fFrame = 0.f;

	SetUp_OnTerrain(m_pTransformCom, 2.f);

	return OBJ_NOEVENT;
}

void CPet::Late_Update(_float fTimeDelta)
{
	Set_BillBoard();

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));

	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix); // 충돌체를 객체의 위치에 맞게 옮겨 주는 함수
}

HRESULT CPet::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (m_ePetState == PET_NORMAL) // 0~3 (4장)
	{
		if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame)))
			return E_FAIL;
	}
	else if (m_ePetState == PET_HEAL) // 4~7 (4장)
	{
		if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame + 4)))
			return E_FAIL;
	}
	else if (m_ePetState == PET_BUFF) // 8~13 (6장)
	{
		if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame + 8)))
			return E_FAIL;
	}
	else if (m_ePetState == PET_FOLLOW) // 14~17 (4장)
	{
		if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_fFrame + 12)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	_float4x4 ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pet_Bean"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	//VI버퍼
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return S_OK;

	//트랜스폼 -> 델타 플레이어 찾으려고 플레이어 찾는거 & 충돌 (그대로)

	CTransform::TRANSFORM_DESC  TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Player")));
	Safe_AddRef(m_pPlayer);

	m_pTransformPlayer = static_cast<CTransform*>(m_pGameInstance->Find_Component(m_eCurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pTransformPlayer);

	// For.Com_Collider_Cube --> 충돌체도 그대로	m_pTransformCom->Set_Billboard();

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

HRESULT CPet::SetUp_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
}

HRESULT CPet::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}

void CPet::Set_BillBoard()
{
	_float4x4 ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3 vScaled = { 0.5f, 0.5f, 0.5f };

	for (size_t i = 0; i < CTransform::STATE_POSITION; i++)
		m_pTransformCom->Set_State(CTransform::STATE(i), *(_float3*)&ViewMatrix.m[i] * *((_float*)&vScaled + i));
}

void CPet::Fly_Route()
{
	// 단위벡터 * 단위벡터 -> 1 * cos 값 (음수가 나올 수 있다)
	// 단위벡터 * 일반벡터 -> 밑변 (음수가 나올 수 있다)

	// 플레이어->펫 벡터 구하기
	m_vPlayerPos = m_pTransformPlayer->Get_State(CTransform::STATE_POSITION);
	m_vPetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vPlayerToPet = m_vPetPos - m_vPlayerPos;

	// 플레이어 룩벡터 구하기
	m_vPlayerLook = m_pTransformPlayer->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&m_vPlayerLook, &m_vPlayerLook);

	// 내적으로 밑변 구하기
	m_fUnderLine = D3DXVec3Dot(&m_vPlayerLook, &m_vPlayerToPet);

	// 플레이어 라이트벡터 구하기
	m_vPlayerRIght = m_pTransformPlayer->Get_State(CTransform::STATE_RIGHT);
	D3DXVec3Normalize(&m_vPlayerRIght, &m_vPlayerRIght);

	// 내적으로 높이 구하기
	m_fHeightLine = D3DXVec3Dot(&m_vPlayerRIght, &m_vPlayerToPet);

	// 범위 밖에 있으면 범위 안으로 이동한다
	// Move() -> 1틱에 움직이는 이동거리
	// GOTO() -> 순간이동

	if (m_fUnderLine > -1.f)
	{
		m_pTransformCom->Move(m_vPlayerLook * -0.1f);
	}
	else if (-2.f > m_fUnderLine)
	{
		m_pTransformCom->Move(m_vPlayerLook * 0.1f);
	}

	if (m_fHeightLine > -1.f)
	{
		m_pTransformCom->Move(m_vPlayerRIght * -0.1f);
	}
	else if (-2.f > m_fHeightLine)
	{
		m_pTransformCom->Move(m_vPlayerRIght * 0.1f);
	}
}

void CPet::Set_PetState()
{
	if (m_pPlayer->Get_HP() < 2) // 피가 1일때
	{
		if (!m_bHpTimeStart) // 피가 부족하기 시작한 시간 측정 시작
		{
			m_bHpTimeStart = true;
			m_dwHpTime = GetTickCount();
		}
		else if (m_bHpTimeStart == true
			&& m_dwHpTime + 10000 < GetTickCount()) // 타이머 시작후, 3초 경과
		{
			m_ePetState = PET_HEAL;
			m_bHpTimeStart = false;
		}
	}
	else // 피를 다시 회복한 경우
	{
		if (m_pPlayer->Get_Fever() < 0.1f) // 피는 충분한데 피버가 2 이하일떄
		{
			if (!m_bFeverTimeStart) // 피가 부족하기 시작한 시간 측정 시작
			{
				m_bFeverTimeStart = true;
				m_dwFeverTime = GetTickCount();
			}
			else if (m_bFeverTimeStart == true
				&& m_dwFeverTime + 10000 < GetTickCount()) // 타이머 시작후, 3초 경과
			{
				m_ePetState = PET_BUFF;
				m_bFeverTimeStart = false;
			}
		}
		else // 피도 충분하고, 피버를 2이상 채운 경우
		{
			m_ePetState = PET_NORMAL;
		}
	}

#pragma region 테스트코드

	if (GetAsyncKeyState('L'))
	{
	}

#pragma endregion


}

void CPet::PetState_Action()
{
	switch (m_ePetState)
	{
	case PET_HEAL:
		if (!m_bCanChangeState)
		{
			m_bCanChangeState = true;
			m_dwStateTime = GetTickCount();
		}
		else if (m_bCanChangeState == true
			&& m_dwStateTime + 2000 < GetTickCount() && m_pPlayer->Get_HP() < 3)
		{
			m_pPlayer->PlusHP(1);
			m_bCanChangeState = false;

			CScreenEffect::EFFECT_DESC Desc;
			Desc.eType = CScreenEffect::EFFECT_TYPE::ET_HEAL;
			Desc.fDuration = 1.f;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScreenEffect"), &Desc)))
				return;

			m_pGameInstance->PlayDefault(TEXT("Pet_Heal.wav"), 31, 0.2f, false);
		}
		break;
	case PET_BUFF:
		if (!m_bCanChangeState)
		{
			m_bCanChangeState = true;
			m_dwStateTime = GetTickCount();
		}
		else if (m_bCanChangeState == true
			&& m_dwStateTime + 2000 < GetTickCount())
		{
			m_pPlayer->Plus_Fever(1.f);
			m_bCanChangeState = false;

			CScreenEffect::EFFECT_DESC Desc;
			Desc.eType = CScreenEffect::EFFECT_TYPE::ET_BUFF;
			Desc.fDuration = 1.f;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScreenEffect"), &Desc)))
				return;

			m_pGameInstance->PlayDefault(TEXT("Pet_Buff.wav"), 31, 0.2f, false);
		}
		break;
	default:
		break;
	}
}

CPet* CPet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPet* pInstance = new CPet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("failed to Create Prototype Nero"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet::Clone(void* pArg)
{
	CPet* pInstance = new CPet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("failed to Clone Nero"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
	Safe_Release(m_pTransformPlayer);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
}
