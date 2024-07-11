#include "stdafx.h"
#include "..\Public\ChaosPatternA.h"

#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Rayzer.h"
#include "Player.h"
#include "Chaos.h"
#include "Interection.h"
#include "UIOrthogonal.h"
#include "Fence.h"
#include "Effect.h"

CChaosPatternA::CChaosPatternA(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject { pGraphic_Device }
{
}

CChaosPatternA::CChaosPatternA(const CChaosPatternA & Prototype)
	: CLandObject { Prototype }
{
}


HRESULT CChaosPatternA::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChaosPatternA::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(10.f, 10.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CChaosPatternA::Priority_Update(_float fTimeDelta)
{

}

int CChaosPatternA::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;

	m_fSumTime += fTimeDelta;

	switch (m_eState)
	{
	case PATTERN_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case PATTERN_IDLE:
		Update_Idle(fTimeDelta);
		break;
	case PATTERN_CHARGE:
		Update_Charge(fTimeDelta);
		break;
	case PATTERN_DEATH:
		Update_Death(fTimeDelta);
		break;
	default:
		break;
	}


	__super::SetUp_OnTerrain(m_pTransformCom, 2.5f);

	return OBJ_NOEVENT;
}

void CChaosPatternA::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		if (m_eState != PATTERN_CHARGE)//레이저 쏘는중이 아니라면 충돌무시
			return;

		static_cast<CInterection*>(m_pInterection)->Set_RenderState(true); // 상호작용키 렌더적용
		if(m_pGameInstance->GetButtonDown(KeyType::E)) //E 키를 누르면 죽게함
		{
			m_eState = PATTERN_DEATH;
			m_pGameInstance->PlayDefault(TEXT("PatternA_Death.wav"), 27, 0.8f, false);
		}
	}
	else
	{
		if(m_pInterection) //충돌중이 아니라면 상호작용UI 끔
			static_cast<CInterection*>(m_pInterection)->Set_RenderState(false);
	}

}

HRESULT CChaosPatternA::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom[m_eState]->Bind_Texture(m_pTextureCom[m_eState]->Get_Frame().iCurrentTex)))
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

void CChaosPatternA::Update_Spawn(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_eState = PATTERN_IDLE;

		_float3 vSpawnPos = __super::Get_Pos();
		vSpawnPos.y -= 0.7;
		CUIOrthogonal::Orthogonal_DESC desc;

		desc.vPos = vSpawnPos;
		desc.fSizeX = 50.f;
		desc.fSizeY = 50.f;
		desc.bCull = false;
		//상호작용 E
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Interection"), TEXT("Prototype_GameObject_Interection"),&desc)))
			return;

		m_pInterection = m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_Interection"))->back();

		//인식표(느낌표)
		vSpawnPos.y += 3.0f;
		desc.vPos = vSpawnPos;
		desc.fSizeX = 40.f;
		desc.fSizeY = 40.f;
		desc.bCull = false;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Indicator"), TEXT("Prototype_GameObject_IndicatorMonster"), &desc)))
			return;

		m_pIndicator = m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_Indicator"))->back();

	}
}

void CChaosPatternA::Update_Idle(_float fTimeDelta)
{
	LEVELID eLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(eLevel, TEXT("Layer_Player")));

	if (!pPlayer)
		return;
	pPlayer->Set_FPSOFF(true);
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_CHARGE;
}

void CChaosPatternA::Update_Charge(_float fTimeDelta)
{
	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);

	LEVELID eLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(eLevel, TEXT("Layer_Player")));

	if (!pPlayer)
		return;
	pPlayer->Set_FPSOFF(true);

}


void CChaosPatternA::Update_Death(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		LEVELID eLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(eLevel, TEXT("Layer_Player")));
		pPlayer->Set_FPSOFF(false);
		if (!pPlayer)
			return;

		

		if (m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_PatternA"))->size() == 1)
		{
			for (auto Fence : *m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_Fence")))
			{
				static_cast<CFence*>(Fence)->Set_Defense(true);
			}
			pPlayer->Set_Pos(_float3(50.f, 0.5f, 18.f));
			CEffect::EFFECT_DESC Desc;
			Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion12");
			Desc.iFrameSpeed = 0.7f;
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Desc)))
				MSG_BOX(TEXT("NO EFFECT"));
			m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(_float3(50.f, 0.5f, 18.f));
			m_pGameInstance->PlayDefault(TEXT("PatternA_Teleport.wav"), 30, 1.f, false);

			static_cast<CChaos*>(m_pGameInstance->Find_Object(LEVEL_CHAOS, TEXT("Layer_Monster")))->Set_State(CChaos::CHAOS_STATE::CHAOS_IDLE);
		}

		m_bDead = true;

	}


	if (m_pRayzer)
		m_pRayzer->Set_Dead(true);

	if (m_pInterection)
		m_pInterection->Set_Dead(true);

	if (m_pIndicator)
		m_pIndicator->Set_Dead(true);




}

HRESULT CChaosPatternA::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 21;
	TextureDesc.fSpeed = 0.05f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternA_Spawn"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_SPAWN]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 54;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternA_Idle"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_IDLE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 49;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternA_Charge"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_CHARGE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 48;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternA_Death"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_DEATH]), &TextureDesc)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
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
	CollCubeDesc.fRadiusY = 0.6f;
	CollCubeDesc.fRadiusX = 0.6f;
	CollCubeDesc.fRadiusZ = 0.6f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChaosPatternA::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);


	return S_OK;
	
}

HRESULT CChaosPatternA::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}


CChaosPatternA * CChaosPatternA::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CChaosPatternA*		pInstance = new CChaosPatternA(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChaosPatternA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CChaosPatternA::Clone(void* pArg)
{
	CChaosPatternA*		pInstance = new CChaosPatternA(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CChaosPatternA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaosPatternA::Free()
{
	__super::Free();

	for(int i = 0; i< PATTERN_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
