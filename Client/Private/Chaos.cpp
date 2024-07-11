#include "stdafx.h"
#include "..\Public\Chaos.h"

#include "GameInstance.h"
#include "Effect.h"
#include "Bullet.h"
#include "Player.h"
#include "ParticleSystem.h"

#include "ChaosPatternA.h"
#include "ChaosPatternB.h"
#include "AttackWay.h"
#include "ChaosBall.h"
#include "Fence.h"
#include "UIOrthogonal.h"
#include "AfterImage.h"
#include "Potal.h"
#include "UIScript.h"

CChaos::CChaos(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEnemy { pGraphic_Device }
{
}

CChaos::CChaos(const CChaos & Prototype)
	: CEnemy{ Prototype }
{  

}


HRESULT CChaos::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChaos::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(50.f, 5.f, 50.f));
	m_pTransformCom->Scaling(20.f, 20.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_CHAOS, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	m_pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_CHAOS, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	m_iHP = 200;

	CPotal::POTAL_DESC PotalDesc;
	_float3 vPos = Get_Pos();
	vPos.y += 7.f;
	vPos.z += -0.5f;
	PotalDesc.vPos = vPos;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Potal"), TEXT("Prototype_GameObject_Potal"), &PotalDesc)))
		MSG_BOX(TEXT("포탈 생성 실패"));

	return S_OK;
}

void CChaos::Priority_Update(_float fTimeDelta)
{
	if(!m_bDead)
		m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
}

int CChaos::Update(_float fTimeDelta)
{
	if (this->m_bDead)
	{
		{
			CUIScript::SCRIPT_DESC Desc;
			Desc.eLevel = LEVEL_CHAOS;
			Desc.iTalker = 1;
			Desc.Script = TEXT("로즈 : 평범한 행성의 생물은 아닌 것 같아.");
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Script"),
				TEXT("Prototype_GameObject_UIScript"), &Desc);
		}
		{
			CUIScript::SCRIPT_DESC Desc;
			Desc.eLevel = LEVEL_CHAOS;
			Desc.iTalker = 0;
			Desc.Script = TEXT("가토 : 이 녀석 네로의 마크가 달려있어.\n네로 제국의 인공생물이었군");
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Script"),
				TEXT("Prototype_GameObject_UIScript"), &Desc);
		}
		{
			CUIScript::SCRIPT_DESC Desc;
			Desc.eLevel = LEVEL_CHAOS;
			Desc.iTalker = 1;
			Desc.Script = TEXT("로즈 : 의뢰비를 두 배로 제안해야겠는걸?");
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Script"),
				TEXT("Prototype_GameObject_UIScript"), &Desc);
		}
		return OBJ_DEAD;
	}

	m_fSumTime += fTimeDelta;

	switch (m_eState)
	{
	case CHAOS_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case CHAOS_IDLE:
		Update_Idle(fTimeDelta);
		break;
	case CHAOS_WALK:
		Update_Walk(fTimeDelta);
		break;
	case CHAOS_MOVE:
		Update_Move(fTimeDelta);
		break;
	case CHAOS_ATTACKA:
		Update_AttackA(fTimeDelta);
		break;
	case CHAOS_ATTACKB:
		Update_AttackB(fTimeDelta);
		break;
	case CHAOS_ATTACKC:
		Update_AttackC(fTimeDelta);
		break;
	case CHAOS_ATTACKD:
		Update_AttackD(fTimeDelta);
		break;
	case CHAOS_DEATH:
		Update_Death(fTimeDelta);
		break;
	default:
		break;
	}

	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);

	
	if (m_pGameInstance->GetButtonDown(KeyType::F1))
		m_eState = CHAOS_DEATH;
	if (m_pGameInstance->GetButtonDown(KeyType::Down))
		m_eState = CHAOS_ATTACKA;
	if (m_pGameInstance->GetButtonDown(KeyType::Up))
		m_eState = CHAOS_ATTACKB;
	if (m_pGameInstance->GetButtonDown(KeyType::Right))
		m_eState = CHAOS_ATTACKC;
	if (m_pGameInstance->GetButtonDown(KeyType::Left))
		m_eState = CHAOS_ATTACKD;


	__super::SetUp_OnTerrain(m_pTransformCom, m_fHeight);

	return OBJ_NOEVENT;
}

void CChaos::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	//m_pTextureCom[CFSM::OBJSTATE_IDLE]->Move_Frame(fTimeDelta);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);


	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		m_bDir = !m_bDir;
	}
	if (m_iHP <= 0)
	{
		m_eState = CHAOS_DEATH;
	}
}

HRESULT CChaos::Render()
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

void CChaos::Update_Spawn(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_eState = CHAOS_IDLE;
		m_fSumTime = 0.f;
	}
		
}

void CChaos::Update_Idle(_float fTimeDelta)
{
	LEVELID eLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
	CGameObject* pPlayer = m_pGameInstance->Find_Object(eLevel, TEXT("Layer_Player"));
	
	if (!pPlayer)
		return;

	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);

	if (m_fHeight < 11.f)
	{
		m_fHeight += 10.f * fTimeDelta;
		return;
	}


	int iPattern;
	if (m_iHP > 100)
	{
		iPattern =2+ (rand() % 2);
	}
	else
	{
		iPattern = (rand() % 2);
	}
	if (m_iPrevPattern == iPattern)
		return;
	if (m_fSumTime < 3.0f)
	{
		return;
	}

	switch (iPattern)
	{
	case 0:
		m_eState = CHAOS_ATTACKA;
		break;
	case 1:
		m_eState = CHAOS_ATTACKB;
		break;
	case 2:
		m_eState = CHAOS_ATTACKC;
		break;
	case 3:
		m_eState = CHAOS_ATTACKD;
		break;
	default:
		break;
	}
	m_iPrevPattern = iPattern;
}

void CChaos::Update_Walk(_float fTimeDelta)
{
	if (m_bOnWater)
	{
		switch (m_iPatternDCount)
		{
		case 1:
			__super::Set_Pos(_float3(27.5f, -6.f, 27.5f));
			break;
		case 2:
			__super::Set_Pos(_float3(77.5f, -6.f, 27.5f));
			break;
		case 3:
			__super::Set_Pos(_float3(77.5f, -6.f, 77.5f));
			break;
		case 4:
			__super::Set_Pos(_float3(27.5f, -6.f, 77.5f));
			break;
		case 5:
			__super::Set_Pos(_float3(50.5f, -6.f, 50.5f));
			break;

		default:
			break;
		}
		m_bOnWater = false;
	}

	//올라오는부분
	if(m_fHeight<4.f)
		m_fHeight += 10.f * fTimeDelta;
	else//다 올라오면 어택 웨이 생성
	{
		if (m_iPatternDCount == 5)
		{
			m_iPatternDCount = 0;
			m_fSumTime = 0.f;
			m_eState = CHAOS_IDLE;
			return;
		}
		CGameObject* pPlayer = nullptr;

		pPlayer = (m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		if (!pPlayer)
		{
			MSG_BOX(TEXT("플레이어 몬찾음 생성 실패"));
			return;
		}

		m_vDir = pPlayer->Get_Pos() - __super::Get_Pos();
		m_vDir.y = 0.f;
		D3DXVec3Normalize(&m_vDir, &m_vDir);

		CAttackWay::ATTACKWAYDESC desc;
		desc.fX = __super::Get_Pos().x;
		desc.fZ = __super::Get_Pos().z;
		desc.fDir = m_vDir;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_PatternD"), TEXT("Prototype_GameObject_AttackWay"), &desc)))
			MSG_BOX(TEXT("어택웨이 생성 실패"));

		m_eState = CHAOS_MOVE;
		m_fSumTime = 0.f;
		m_pGameInstance->PlayDefault(TEXT("PatternD_Start.wav"), 30, 1.f, false);
	}

	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);
}

void CChaos::Update_Move(_float fTimeDelta)
{
	m_fAfterImageTime += fTimeDelta;
	if (m_fAfterImageTime > 0.05f)
	{
		m_fAfterImageTime = 0.f;

		CAfterImage::AFTER_DESC Desc;
		Desc.vPos = Get_Pos();
		Desc.iCurTexIndex = m_pTextureCom[m_eState]->Get_Frame().iCurrentTex;
		Desc.pTextureTag = TEXT("Prototype_Component_Texture_Chaos_Move");
		Desc.eTextureLevel = LEVEL_CHAOS;
		Desc.iType = 3;
		Desc.Scale = _float3(20.f, 20.f, 1.f);
		m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_AfterImage"), TEXT("Prototype_GameObject_AfterImage"), &Desc);
	}
	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);

	if (m_fSumTime > 0.5f)
	{
		m_pTransformCom->Go_PosDir(fTimeDelta * 40.f, m_vDir);

		if (__super::Get_Pos().x <= 27.0f || __super::Get_Pos().x >= 78.0f
			|| __super::Get_Pos().z <= 27.0f || __super::Get_Pos().z >= 78.0f)
		{

			m_eState = CHAOS_ATTACKD;
			m_fSumTime = 0.f;
		}
	}
	else
	{

	}
}

void CChaos::Update_AttackA(_float fTimeDelta)
{

	if (m_iPatternACount == 0 && m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		++m_iPatternACount;
		m_pTextureCom[m_eState]->Reset_Frame();
		CLandObject::LANDOBJECT_DESC	Desc = {};

		Desc.pTerrainTransform = m_pTerrainTransform;
		Desc.pTerrainVIBuffer = m_pTerrainVIBuffer;
		Desc.eLevel = LEVEL_CHAOS;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_PatternA"), TEXT("Prototype_GameObject_ChaosPatternA"), &Desc)))
			MSG_BOX(TEXT("패턴1 생성 실패"));
		m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_PatternA"))->back()->Set_Pos(_float3(5.f, 25.f, 5.f));
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_PatternA"), TEXT("Prototype_GameObject_ChaosPatternA"), &Desc)))
			MSG_BOX(TEXT("패턴1 생성 실패"));
		m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_PatternA"))->back()->Set_Pos(_float3(5.f, 25.f, 95.f));
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_PatternA"), TEXT("Prototype_GameObject_ChaosPatternA"), &Desc)))
			MSG_BOX(TEXT("패턴1 생성 실패"));
		m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_PatternA"))->back()->Set_Pos(_float3(95.f, 25.f, 95.f));
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_PatternA"), TEXT("Prototype_GameObject_ChaosPatternA"), &Desc)))
			MSG_BOX(TEXT("패턴1 생성 실패"));
		m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_PatternA"))->back()->Set_Pos(_float3(95.f, 25.f, 5.f));



		for (auto Fence : *m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_Fence")))
		{
			static_cast<CFence*>(Fence)->Set_Defense(false);
		}
	}
	else if(m_iPatternACount>0)
	{
		if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta*1.5f, false))
		{
			m_pTextureCom[m_eState]->Reset_Frame();
			CGameObject* pPlayer = nullptr;

			pPlayer = (m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

			CChaosBall::ChaosBallDesc Desc;
			Desc.vPos = Get_Pos();
			Desc.vPos.y = pPlayer->Get_Pos().y;

			Desc.vLook = pPlayer->Get_Pos() - Get_Pos();
			Desc.vLook.y = 0;
			_float3 vRight;
			D3DXVec3Cross(&vRight, &Desc.vLook, &_float3(0.f, 1.f, 0.f));

			D3DXVec3Normalize(&vRight, &vRight);
			_float3 pos = Desc.vPos;
			for (int i = -9; i < 9; ++i)
			{
				Desc.vPos = pos + vRight * i;

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_ChaosBall"), TEXT("Prototype_GameObject_ChaosBall"), &Desc)))
					MSG_BOX(TEXT("카오스볼 생성 실패"));
			}
		}
		
	}

}

void CChaos::Update_AttackB(_float fTimeDelta)
{
	if (m_fSumTime > 1.f)
	{
		m_fSumTime = 0.f;
		m_pGameInstance->PlayDefault(TEXT("PatternB.wav"), 30, 1.f, false);
		CChaosPatternB::PATTERN_DESC	Desc = {};
		for (int i = 0; i < 36; ++i)
		{
			_float angle = i * 10.f; // 0도부터 350도까지 10도씩 증가
			_float radius = 8.0f + (m_iPatternBCount*3); // 예시로 설정한 반지름

			// 원형 내에 균일하게 배치하기 위해 원의 방정식을 사용
			_float x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x + radius * cos(D3DXToRadian(angle));
			_float z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z + radius * sin(D3DXToRadian(angle));

			Desc.vTargetPos = _float3(x, 0.5f, z);
			Desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Pattern"), TEXT("Prototype_GameObject_ChaosPatternB"), &Desc)))
				MSG_BOX(TEXT("패턴2 생성 실패"));
		}
		++m_iPatternBCount;

		
		if(m_iPatternBCount>=7)
		{
			m_eState = CHAOS_IDLE;
			m_fSumTime = 0.f;
			m_iPatternBCount = 0;
		}
	}


	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);
}

void CChaos::Update_AttackC(_float fTimeDelta)
{

	if (m_fSumTime >2.f)
	{
		m_fSumTime = 0.f;

		CLandObject::LANDOBJECT_DESC	Desc = {};
		Desc.pTerrainTransform = m_pTerrainTransform;
			Desc.pTerrainVIBuffer = m_pTerrainVIBuffer;
		Desc.eLevel = LEVEL_CHAOS;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Pattern"), TEXT("Prototype_GameObject_ChaosPatternC"), &Desc)))
			MSG_BOX(TEXT("패턴3 생성 실패"));

		++m_iPatternCCount;

		if (m_iPatternCCount >= 6)
		{
			m_eState = CHAOS_IDLE;
			m_fSumTime = 0.f;
			m_iPatternCCount = 0;
		}
	}

	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);
}

void CChaos::Update_AttackD(_float fTimeDelta)
{	
	m_fHeight -= 2.5*fTimeDelta;
	if (m_bPatternFirst == false)
	{
		m_pGameInstance->PlayDefault(TEXT("PatternD_Down.wav"), 29, 0.8f, false);
		m_bPatternFirst = true;
	}
	
	if (m_fSumTime >= 0.1f&& m_fHeight > -2.5f)
	{
		

		_float3 vPos = Get_Pos();
		vPos.y = 0.3f;
		CParticleSystem::STATE_DESC ParticleDesc;
		ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATE_DESC));
		ParticleDesc.eType = CParticleSystem::PARTICLE_FLARE;
		ParticleDesc.eTextureLevel = LEVEL_CHAOS;
		ParticleDesc.strTextureTag = TEXT("Prototype_Component_Texture_WaterParticle");
		ParticleDesc.dDuration = 0.4f;
		ParticleDesc.dParticleLifeTime = 0.5f;
		ParticleDesc.dSpawnTime = 1;
		ParticleDesc.fParticlePerSecond = 75;
		ParticleDesc.fVelocityDeviation = 1.f;
		ParticleDesc.iMaxParticleCount = 15;
		ParticleDesc.vParticleScale = _float2(0.5f, 0.5f);
		ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
		ParticleDesc.iTextureNum =0;
		ParticleDesc.vVelocity = _float3((rand() % 10) * 0.1f, (rand() % 10) * 0.1f, rand() % 10 * 0.1f);
		ParticleDesc.vPosition = vPos;
		if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Particle"), TEXT("ProtoType_ParicleSystem"), &ParticleDesc)))
			MSG_BOX(TEXT("no particle"));

		m_fSumTime = 0.f;

	}

	if (m_fHeight < -6.f)
	{
		++m_iPatternDCount;
		m_eState = CHAOS_WALK;
		m_bOnWater = true;
		m_bPatternFirst = false;
	}

	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);
}

void CChaos::Update_Death(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_bDead = true;
		for (auto Fence : *m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_Fence")))
		{
			static_cast<CFence*>(Fence)->Set_Defense(false);
		}

		CUIOrthogonal::Orthogonal_DESC Desc;
		Desc.vPos = _float3(50.f, 5.f, 98.5f);
		Desc.fSizeX = 30.f;
		Desc.fSizeY = 30.f;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_UI"),
			TEXT("Prototype_GameObject_NextArrow"), &Desc)))
				MSG_BOX(TEXT("화살표 못만듬"));

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Background"),
			TEXT("Prototype_GameObject_MapWall"))))
			MSG_BOX(TEXT("문 못만듬"));

		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Get_ObjectList(LEVEL_CHAOS, TEXT("Layer_Background"))->back()->Find_Component(TEXT("Com_Transform")));
		if (pTransform)
		{
			pTransform->Scaling(2.5f, 4.f, 1.f);
			pTransform->Set_State(CTransform::STATE_POSITION, _float3(50.f, 2.f, 98.8f));
		}


	}
	if (m_pTextureCom[m_eState]->Get_Frame().iCurrentTex == 20)
		m_pGameInstance->PlayDefault(TEXT("Chaos_Dead.wav"), 30, 0.9f, false);
		
}

void CChaos::IsPicked(_float3* pOutPos, _float* pDist)
{
	if (m_bDead)
		return;
	 
	_float2 vPos = _float2(pOutPos->x, pOutPos->y);
	if (D3DXVec2Length(&vPos) >= 0.25f)
		return;

	//float f = (input.texcoord.x - 0.5-xSplit) * (input.texcoord.x - 0.5-xSplit) + (input.texcoord.y - 0.5) * (input.texcoord.y - 0.5) - 0.3 * 0.3;
	_float3 vHitingPos;
	D3DXVec3TransformCoord(&vHitingPos, pOutPos, &m_pTransformCom->Get_WorldMatrix());

	CPlayer* pPlayer = nullptr;

	pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	if (pPlayer == nullptr)
		return;

	if (pPlayer->Get_Weapon() == CPlayer::WEAPONS::RAMBO)
	{
		m_iHP--;
	}

	else if (pPlayer->Get_Weapon() == CPlayer::WEAPONS::SPARROW)
	{
		m_iHP -= 5;
	}

	CEffect::EFFECT_DESC EffectDesc;
	EffectDesc.iFrameSpeed = 0.3f;
	EffectDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion8");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &EffectDesc)))
		MSG_BOX(TEXT("NO EFFECT"));

	m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(vHitingPos);

	CParticleSystem::STATE_DESC ParticleDesc;
	ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATE_DESC));
	ParticleDesc.eType = CParticleSystem::PARTICLE_TEST;
	ParticleDesc.eTextureLevel = LEVEL_GAMEPLAY;
	ParticleDesc.strTextureTag = TEXT("Prototype_Component_Texture_Particle");
	ParticleDesc.dDuration = 0.4f;
	ParticleDesc.dParticleLifeTime = 1.2f;
	ParticleDesc.dSpawnTime = 1;
	ParticleDesc.fParticlePerSecond = 75;
	ParticleDesc.fVelocityDeviation = 1.f;
	ParticleDesc.iMaxParticleCount = 2;
	ParticleDesc.vParticleScale = _float2(0.9f, 0.9f);
	ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
	ParticleDesc.iTextureNum = 2;
	ParticleDesc.vVelocity = _float3((rand() % 10) * 0.1f, (rand() % 10) * 0.1f, rand() % 10 * 0.1f);
	ParticleDesc.vPosition = vHitingPos;
	ParticleDesc.vPosition.z += (_float)0.1;
	if (FAILED(CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Particle"), TEXT("ProtoType_ParicleSystem"), &ParticleDesc)))
		MSG_BOX(TEXT("no particle"));


	pPlayer->Plus_Fever(0.1f);
}

HRESULT CChaos::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 89;
	TextureDesc.fSpeed = 0.08f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Spawn"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_SPAWN]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 150;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Idle"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_IDLE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 199;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Walk"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_WALK]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 79;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Move"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_MOVE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 182;
	TextureDesc.fSpeed = 0.005f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_Death"),
		TEXT("Com_Texture8"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_DEATH]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 80;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_AttackA"),
		TEXT("Com_Texture4"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_ATTACKA]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 127;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_AttackB"),
		TEXT("Com_Texture5"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_ATTACKB]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 99;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_AttackC"),
		TEXT("Com_Texture6"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_ATTACKC]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 199;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_AttackD"),
		TEXT("Com_Texture7"), reinterpret_cast<CComponent**>(&m_pTextureCom[CHAOS_ATTACKD]), &TextureDesc)))
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

HRESULT CChaos::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);


	return S_OK;
	
}

HRESULT CChaos::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);



	return S_OK;
}


CChaos * CChaos::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CChaos*		pInstance = new CChaos(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChaos"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CChaos::Clone(void* pArg)
{
	CChaos*		pInstance = new CChaos(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CChaos"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaos::Free()
{
	__super::Free();

	for(int i = 0; i< CHAOS_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
