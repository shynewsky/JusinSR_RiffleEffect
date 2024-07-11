#include "stdafx.h"
#include "Enemy.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Effect.h"
#include "Player.h"

CEnemy::CEnemy(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CLandObject{pGraphic_Device}
{
}

CEnemy::CEnemy(const CEnemy& Prototype)
	:CLandObject{ Prototype }
{
}

HRESULT CEnemy::Initialize(void* pArg)
{
	ENEMY_DESC* pDesc = static_cast<ENEMY_DESC*>(pArg);

	m_pTerrainVIBuffer = pDesc->pTerrainVIBuffer;
	Safe_AddRef(m_pTerrainVIBuffer);

	m_pTerrainTransform = pDesc->pTerrainTransform;
	Safe_AddRef(m_pTerrainTransform);

	m_eCurLevel = pDesc->eLevel;
	return S_OK;
}

void CEnemy::IsPicked(_float3* pOutPos, _float* pDist)
{
	if (m_bDead)
		return;

	_float2 vPos = _float2(pOutPos->x, pOutPos->y);
	if (D3DXVec2Length(&vPos) >= 0.3f)
		return;

	_float3 vHitingPos;
	D3DXVec3TransformCoord(&vHitingPos, pOutPos, &m_pTransformCom->Get_WorldMatrix());

	CEffect::EFFECT_DESC Desc;

	CPlayer* pPlayer = nullptr;

	pPlayer = static_cast<CPlayer*>(m_pPlayer);

	if (pPlayer == nullptr)
		return;

	if (pPlayer->Get_Weapon() == CPlayer::WEAPONS::RAMBO)
	{
		Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_RifleEffect");
		m_iHP--;
	}

	else if (pPlayer->Get_Weapon() == CPlayer::WEAPONS::SPARROW)
	{
		Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_SniperEffect");
		m_iHP -= 5;
	}

	Desc.iFrameSpeed = 0.3f;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Desc)))
		MSG_BOX(TEXT("NO EFFECT"));

	m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(vHitingPos);

	static_cast<CPlayer*>(m_pPlayer)->Plus_Fever(0.1f);

	m_iHP--;
}

void CEnemy::Reset_Invertia()
{
	ZeroMemory(m_vInertia, sizeof(_float3));
}

void CEnemy::KillSwitch()
{
	m_iHP = 0;
}

CGameObject* CEnemy::Clone(void* pArg)
{
	return nullptr;
}

void CEnemy::Friction(_float _FrictionRate)
{
	m_vInertia.x *= _FrictionRate;
	m_vInertia.z *= _FrictionRate;
	_float2 Dest = { m_vInertia.x , m_vInertia.z };
	if (D3DXVec2Length(&Dest) < 0.01)
	{
		m_vInertia.x = 0.f;
		m_vInertia.z = 0.f;
	}
}

void CEnemy::Compute_DeltaPlayer()
{
	if (nullptr == m_pPlayer)
		return;

	m_vDeltaPlayer = m_pPlayer->Get_Pos() - Get_Pos();
	m_fDeltaPlayer = D3DXVec3Length(&m_vDeltaPlayer);
}

void CEnemy::repulsion()
{
	for (auto obj : *m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_Monster")))
	{
		if (nullptr != obj)
		{
			_float3 Dest = (obj->Get_Pos() - Get_Pos());
			if ( D3DXVec3Length(&Dest) < 1.f)
			{
				D3DXVec3Normalize(&Dest, &Dest);
				m_pTransformCom->GOTO(obj->Get_Pos() - Dest * 1.f);
			}
		}
	}
}

void CEnemy::Free()
{
	__super::Free();
	if (nullptr != m_pFSM)
	{
		CFSM::Destroy(m_pFSM);
		m_pFSM = nullptr;
	}
	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pTransformPlayer);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	
	
}