#include "stdafx.h"
#include "Boss_Nero.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Player.h"

//#include "Bullet_Arc.h"
#include "Bullet_Sphere.h"
#include "Bullet_Cube.h"
#include "Bullet_Corn.h"
#include "Bar_Cylinder.h"
#include "Beam_Cylinder.h"
#include "Razor_Cylinder.h"
#include "Pull_Pyramid.h"
#include "Greek_Column.h"
#include "Round_Fence.h"
#include "Stone_Stand.h"

#include "Effect.h"
#include "AfterImage.h"
#include "TriggerBox.h"
#include "UIOrthogonal.h"
#include "UIScript.h"

CBoss_Nero::CBoss_Nero(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CEnemy{ pGraphic_Device }
{
}

CBoss_Nero::CBoss_Nero(const CBoss_Nero& Prototype)
	:CEnemy{ Prototype }
{
}

HRESULT CBoss_Nero::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Nero::Initialize(void* pArg)
{
	if (__super::Initialize(pArg)) // LandObject 상속을 통해서 터레인 정보를 가져온다
		return E_FAIL;

	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	ENEMY_DESC Desc = *(ENEMY_DESC*)pArg;

	//m_pTransformCom->Scaling(Desc.fScale.x, Desc.fScale.y, Desc.fScale.z);

	m_pTransformCom->GOTO(Desc.vDir); // 위치벡터

	m_iHP = m_iMaxHP = Desc.iMaxHP; // 최대 HP를 설정하고

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix(); // 충돌체에 내 월드행렬을 부여

	return S_OK;
}

void CBoss_Nero::Priority_Update(_float fTimeDelta)
{
	if(!m_bDead)
		m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this); // 마우스피킹으로 총알을 맞을 그룹

	Compute_DeltaPlayer(); // 몬스터가 플레이어를 바라보는 벡터
}

int CBoss_Nero::Update(_float fTimeDelta)
{
	if (m_iHP <= 0)
	{
		if (!m_bFinalScript)
		{
			CUIScript::SCRIPT_DESC Desc;
			Desc.eLevel = LEVEL_NERO;
			Desc.iTalker = 0;
			Desc.Script = TEXT("가토 : 의뢰는 성공적으로 완료됐다.\n 이제 이 행성도 다시 평화를 되찾을 수 있을\n거야.");
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_NERO, TEXT("Layer_Script"),
				TEXT("Prototype_GameObject_UIScript"), &Desc);
			m_bFinalScript = true;
		}
		m_pFSM->Set_State(CFSM::OBJSTATE_DIE);
	}
	Set_ObjState();

	Set_AttackPattern();

	SetUp_UnderTerrain(m_pTransformCom, 0.5f);
	m_fSumTime += fTimeDelta;

	if (m_bDead)
		Dead_Animation();
	//return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CBoss_Nero::Late_Update(_float fTimeDelta)
{
	m_pFSM->Late_Update(fTimeDelta); // 프레임 돌리는 MoveFrame 내장

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	CameraAround(); // 내적 두번하는 함수

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));

	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix); // 충돌체를 객체의 위치에 맞게 옮겨 주는 함수

	Set_BillBoard_Nero();
}

HRESULT CBoss_Nero::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom[m_pFSM->Get_CurState()]->Bind_Texture(m_pTextureCom[m_pFSM->Get_CurState()]->Get_Frame().iCurrentTex)))
		return E_FAIL; // 그대로 써라

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

HRESULT CBoss_Nero::Ready_Components()
{
	//텍스쳐
	CTexture::TEXTUREDESC   TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Appear"),
		TEXT("Com_Texture0", &(int)CFSM::OBJSTATE_APPEAR), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_APPEAR]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_APPEAR, m_pTextureCom[CFSM::OBJSTATE_APPEAR]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_DIsappear"),
		TEXT("Com_Texture9", &(int)CFSM::OBJSTATE_DIE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_DIE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_DIE, m_pTextureCom[CFSM::OBJSTATE_DIE]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Fly"),
		TEXT("Com_Texture1", &(int)CFSM::OBJSTATE_FLY), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_FLY]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_FLY, m_pTextureCom[CFSM::OBJSTATE_FLY]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_FlyDown"),
		TEXT("Com_Texture2", &(int)CFSM::OBJSTATE_FLYDOWN), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_FLYDOWN]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_FLYDOWN, m_pTextureCom[CFSM::OBJSTATE_FLYDOWN]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_FlyUp"),
		TEXT("Com_Texture3", &(int)CFSM::OBJSTATE_FLYUP), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_FLYUP]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_FLYUP, m_pTextureCom[CFSM::OBJSTATE_FLYUP]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Groggy"),
		TEXT("Com_Texture4", &(int)CFSM::OBJSTATE_GROGGY), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_GROGGY]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_GROGGY, m_pTextureCom[CFSM::OBJSTATE_GROGGY]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Idle"),
		TEXT("Com_Texture5", &(int)CFSM::OBJSTATE_IDLE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Move"),
		TEXT("Com_Texture6", &(int)CFSM::OBJSTATE_MOVE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_MOVE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_MOVE, m_pTextureCom[CFSM::OBJSTATE_MOVE]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_PatternStart"),
		TEXT("Com_Texture7", &(int)CFSM::OBJSTATE_PATTERNSTART), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_PATTERNSTART]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_PATTERNSTART, m_pTextureCom[CFSM::OBJSTATE_PATTERNSTART]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Throw"),
		TEXT("Com_Texture8", &(int)CFSM::OBJSTATE_THROW), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_THROW]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_THROW, m_pTextureCom[CFSM::OBJSTATE_THROW]);

	if (FAILED(__super::Add_Component(LEVEL_NERO, TEXT("Prototype_Component_Texture_Nero_Dead"),
		TEXT("Com_Texture10", &(int)CFSM::OBJSTATE_DEAD), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_DEAD]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_DEAD, m_pTextureCom[CFSM::OBJSTATE_DEAD]);

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

	m_pPlayer = m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Player"));
	Safe_AddRef(m_pPlayer);

	m_pTransformPlayer = static_cast<CTransform*>(m_pGameInstance->Find_Component(m_eCurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pTransformPlayer);

	// For.Com_Collider_Cube --> 충돌체도 그대로

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

HRESULT CBoss_Nero::SetUp_RenderState()
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

HRESULT CBoss_Nero::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}

HRESULT CBoss_Nero::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 10, 1 }; // 8장 8방향
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_APPEAR, Desc);

	Desc = { 10, 1 }; // 8장 8방향
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_DIE, Desc);

	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_FLY, Desc);

	Desc = { 5, 8 }; // 5장 8방향
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_FLYDOWN, Desc);

	Desc = { 5, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_FLYUP, Desc);

	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_GROGGY, Desc);

	Desc = { 16, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_MOVE, Desc);

	Desc = { 8, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_PATTERNSTART, Desc);

	Desc = { 12, 8 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_THROW, Desc);

	Desc = { 8, 1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_DEAD, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_APPEAR);
	return S_OK;
}

void CBoss_Nero::CameraAround()
{
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_mCamreraWorld);

	D3DXMatrixInverse(&m_mCamreraWorld, nullptr, &m_mCamreraWorld);

	//cos22.5 = 0.923880
	//cos67.5 = 0.382683
	_float zDot = D3DXVec3Dot(&m_vDeltaPlayer, &(*(_float3*)(&m_mCamreraWorld._31)));

	if (zDot >= 0.9238)
	{
		m_pFSM->Turn_Frame(WAY_BACK);
		return;
	}
	else if (zDot < -0.9238)
	{
		m_pFSM->Turn_Frame(WAY_FRONT);
		return;
	}

	_float xDot = D3DXVec3Dot(&m_vDeltaPlayer, &(*(_float3*)(&m_mCamreraWorld._11)));

	if (zDot >= 0.3826)
	{
		if (xDot > 0)
		{
			m_pFSM->Turn_Frame(WAY_BACKR);
			return;
		}

		if (xDot < 0)
		{
			m_pFSM->Turn_Frame(WAY_BACKL);
			return;
		}
	}

	if (zDot < -0.3826)
	{
		if (xDot > 0)
		{
			m_pFSM->Turn_Frame(WAY_FRONTR);
			return;
		}

		if (xDot < 0)
		{
			m_pFSM->Turn_Frame(WAY_FRONTL);
			return;
		}
	}

	if (zDot < 0.3826)
	{
		if (xDot > 0)
		{
			m_pFSM->Turn_Frame(WAY_R);
			return;
		}

		if (xDot < 0)
		{
			m_pFSM->Turn_Frame(WAY_L);
			return;
		}
	}
}

void CBoss_Nero::Set_BillBoard_Nero()
{
	_float4x4 ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3 vScaled = { 1.f, 1.f, 1.f };

	for (size_t i = 0; i < CTransform::STATE_POSITION; i++)
		m_pTransformCom->Set_State(CTransform::STATE(i), *(_float3*)&ViewMatrix.m[i] * *((_float*)&vScaled + i));
}

HRESULT CBoss_Nero::Set_ObjState()
{
	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_APPEAR
		&& (m_dwStateTime + 3000 < GetTickCount()))
	{
		m_pFSM->Set_State(CFSM::OBJSTATE_FLYUP);

		m_pGameInstance->StopSound(25);
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Play3D(TEXT("Nero_FlyUp.wav"), 22, 1.f, vPos, false);
		m_pGameInstance->Set_SoundPos(22, vPos);

		m_dwStateTime = GetTickCount();
	}

	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_FLYUP)
	{
		_float3 fCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, fCurPos+_float3(0.f, 0.1f, 0.f));

		if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y > m_fMaxFlyHeight)
		{
			m_pFSM->Set_State(CFSM::OBJSTATE_PATTERNSTART);

			_float fCurX = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
			_float fCurZ = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(fCurX, 5.f, fCurZ));

			m_dwStateTime = GetTickCount();
		}
	}

	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_PATTERNSTART)
	{
		if (m_dwStateTime + 4000 < GetTickCount())
		{
			m_pFSM->Set_State(CFSM::OBJSTATE_FLY);
			m_dwStateTime = GetTickCount();

			m_pGameInstance->StopSound(22);
			_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->Play3D(TEXT("Nero_Fly.wav"), 23, 1.f, vPos, true);
			m_pGameInstance->Set_SoundPos(23, vPos);

			m_bPatternStart = true;
		}
	}

	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_FLY)
	{
		if (m_dwStateTime + 30000 < GetTickCount())
		{
			m_pFSM->Set_State(CFSM::OBJSTATE_FLYDOWN);
			m_dwStateTime = GetTickCount();

			m_pGameInstance->StopSound(23);
			_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->Play3D(TEXT("Nero_FlyDown.wav"), 24, 1.f, vPos, false);
			m_pGameInstance->Set_SoundPos(24, vPos);

			m_bPatternStart = false;
		}
	}

	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_FLYDOWN)
	{
		_float3 fCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, fCurPos - _float3(0.f, 0.1f, 0.f));

		if (m_dwStateTime + 3000 < GetTickCount())
		{
			m_pFSM->Set_State(CFSM::OBJSTATE_GROGGY);

			_float fCurX = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
			_float fCurZ = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(fCurX, 0.f, fCurZ));

			m_pGameInstance->StopSound(24);
			_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->Play3D(TEXT("Nero_Groggy.wav"), 25, 1.f, vPos, false);
			m_pGameInstance->Set_SoundPos(25, vPos);

			m_dwStateTime = GetTickCount();
		}
	}

	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_GROGGY)
	{
		if (m_dwStateTime + 5000 < GetTickCount())
		{
			m_pFSM->Set_State(CFSM::OBJSTATE_FLYUP);

			m_dwStateTime = GetTickCount();
		}
	}

	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_DIE)
	{
		_float3 fCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, fCurPos - _float3(0.f, 0.1f, 0.f));
		if (m_bDeadSound)
		{
			m_pGameInstance->PlayDefault(TEXT("Nero_Dead.wav"), 19, 0.8f, false);
			m_bDeadSound = false;
		}
			
		if (m_dwStateTime + 5000 < GetTickCount())
		{
			m_pFSM->Set_State(CFSM::OBJSTATE_DEAD);
		}
	}

	if (m_pFSM->Get_CurState() == CFSM::OBJSTATE_DEAD)
	{
		if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < m_fMaxStandHeight)
		{
			_float3 fCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, fCurPos + _float3(0.f, 0.1f, 0.f));

			m_bDead = true;
		}
	}

	return S_OK;
}

void CBoss_Nero::Set_AttackPattern()
{
	if (m_bPatternStart)
	{
		switch (m_dwPatternTime % 8)	
		{
		case 0 :
			if (!m_bSkillStart)
			{
				Skill_BulletCurtain();
				m_bSkillStart = true;
			}
			else
			{
				Fly_Route();

				if (m_dwPatternTime + 2000 < GetTickCount())
				{
					m_bSkillStart = false;
					m_dwPatternTime = GetTickCount();
				}
			}
			break;

		case 1 :
			if (!m_bSkillStart)
			{
				Skill_BulletShoot();
				m_bSkillStart = true;
			}
			else
			{
				if (m_dwPatternTime + 2000 < GetTickCount())
				{
					m_bSkillStart = false;
					m_dwPatternTime = GetTickCount();
				}
			}
			break;

		case 2 :
			if (!m_bSkillStart)
			{
				if (m_iEffectCount < 3
					&& (m_dwEffectTime + 2000 < GetTickCount()) )
				{
					Effect_Before_Beam();
					m_iEffectCount++;
					m_dwEffectTime = GetTickCount();
				}
				else
				{
					Skill_Beam();
					m_bSkillStart = true;
				}
			}
			else
			{
				if (m_dwPatternTime + 5000 < GetTickCount())
				{
					m_iEffectCount = 0;
					m_bSkillStart = false;
					m_dwPatternTime = GetTickCount();
				}
			}
			break;

		case 3 :
			if (!m_bSkillStart)
			{
				Skill_WallDash();
				m_bSkillStart = true;
			}
			else
			{
				Fly_Route();

				if (m_dwPatternTime + 5000 < GetTickCount())
				{
					m_bSkillStart = false;
					m_dwPatternTime = GetTickCount();
				}
			}
			break;

		case 4 :
			if (!m_bSkillStart)
			{
				Skill_IceDrop();
				m_bSkillStart = true;
			}
			else
			{
				Fly_Route();

				if (m_dwPatternTime + 2000 < GetTickCount())
				{
					m_bSkillStart = false;
					m_dwPatternTime = GetTickCount();
				}
			}
			break;

		case 5 :
			if (!m_bSkillStart)
			{
				if (m_iEffectCount < 3
					&& (m_dwEffectTime + 2000 < GetTickCount()))
				{
					Effect_Before_Pull();
					m_iEffectCount++;
					m_dwEffectTime = GetTickCount();
				}
				else
				{
					Skill_Pull();
					m_bSkillStart = true;
				}
			}
			else
			{
				if (m_dwPatternTime + 5000 < GetTickCount())
				{
					m_iEffectCount = 0;
					m_bSkillStart = false;
					m_dwPatternTime = GetTickCount();
				}
			}
			break;

		case 6 :
			if (!m_bSkillStart)
			{
				Skill_RazorLine();
				m_bSkillStart = true;
			}
			else
			{
				Fly_Route();

				if (m_dwPatternTime + 5000 < GetTickCount())
				{
					m_bSkillStart = false;
					m_dwPatternTime = GetTickCount();
				}
			}
			break;

		case 7 :
			if (!m_bSkillStart)
			{
				Skill_ClockHand();
				m_bSkillStart = true;
			}
			else
			{
				Fly_Route();

				if (m_dwPatternTime + 2000 < GetTickCount())
				{
					m_bSkillStart = false;
					m_dwPatternTime = GetTickCount();
				}
			}
			break;

		default:
			break;
		}
	}
}

void CBoss_Nero::Fly_Route()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3( 50.f + (30.f * cos(m_fCurveRate * m_fDegreeAngle) * cos(m_fDegreeAngle)),
			m_pTransformCom->Get_State(CTransform::STATE_POSITION).y,
			50.f + (30.f * cos(m_fCurveRate * m_fDegreeAngle) * sin(m_fDegreeAngle))));

	m_fDegreeAngle += 0.007f;

	if (m_fSumTime > 0.05f)
	{
		m_fSumTime = 0.f;
		CAfterImage::AFTER_DESC Desc;
		Desc.vPos = Get_Pos();
		Desc.iCurTexIndex = m_pFSM->Get_CurTextureCom()->Get_Frame().iCurrentTex;
		Desc.pTextureTag = TEXT("Prototype_Component_Texture_Nero_Fly");
		Desc.eTextureLevel = LEVEL_NERO;
		Desc.iType = 4;
		Desc.Scale = _float3(1.f, 1.f, 1.f);
		m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_AfterImage"), TEXT("Prototype_GameObject_AfterImage"), &Desc);
	}

}

void CBoss_Nero::Dead_Animation()
{
	if (!m_bDeadStart)
	{
		Greek_Column();
		Round_Fence();
		Stone_Stage();

		m_bDeadStart = true;
		CTriggerBox::TRIGGER_INFO trigerinfo;
		trigerinfo.eType = CTriggerBox::ET_CHANGE_LEVEL;
		trigerinfo.pos = _float3(50.f, 1.f, 1.f);

		//트리커 박스
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_NERO, TEXT("Layer_TriggerBox"), TEXT("Prototype_GameObject_TriggerBox"), &trigerinfo)))
			MSG_BOX(TEXT("노 트리거 박스"));

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_NERO, TEXT("Layer_Background"),
			TEXT("Prototype_GameObject_MapWall"))))
			MSG_BOX(TEXT("문 못만듬"));

		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Get_ObjectList(LEVEL_NERO, TEXT("Layer_Background"))->back()->Find_Component(TEXT("Com_Transform")));
		if (pTransform)
		{
			pTransform->Scaling(2.5f, 4.f, 1.f);
			pTransform->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(180));
			pTransform->Set_State(CTransform::STATE_POSITION, _float3(50.f, 2.f, 0.4f));
		}

		CUIOrthogonal::Orthogonal_DESC Desc;
		Desc.vPos = _float3(50.f, 5.f, 1.f);
		Desc.fSizeX = 30.f;
		Desc.fSizeY = 30.f;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_NERO, TEXT("Layer_UI"),
			TEXT("Prototype_GameObject_NextArrow"), &Desc)))
			MSG_BOX(TEXT("화살표 못만듬"));
	}
	else
	{
		if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < m_fMaxStandHeight)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION,
				m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _float3(0.f, 0.1f, 0.f));
	}
}

void CBoss_Nero::Greek_Column()
{
	CGreek_Column::BULLET_DESC Desc = {};

	for (int i = 0; i < 5; i++)
	{
		Desc = {
			_float3(
				m_pTransformCom->Get_State(CTransform::STATE_POSITION).x + (5 * cos(D3DXToRadian(72 * i))),
				-4.f,
				m_pTransformCom->Get_State(CTransform::STATE_POSITION).z + (5 * sin(D3DXToRadian(72 * i)))),

			_float3(1.f, 8.f, 1.f),

			_float3(0.f, 1.f, 0.f),

			4.f
		};

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
			TEXT("Prototype_GameObject_Greek_Column"), &Desc)))
			return;
	}
}

void CBoss_Nero::Round_Fence()
{
	CRound_Fence::BULLET_DESC Desc = {};

	Desc = {
		_float3(
			m_pTransformCom->Get_State(CTransform::STATE_POSITION).x,
			-5.f,
			m_pTransformCom->Get_State(CTransform::STATE_POSITION).z),

		_float3(10.f, 6.f, 10.f),

		_float3(0.f, 1.f, 0.f),

		3.f
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Round_Fence"), &Desc)))
		return;
}

void CBoss_Nero::Stone_Stage()
{
	CStone_Stand::BULLET_DESC Desc = {};

	Desc = {
		m_pTransformCom->Get_State(CTransform::STATE_POSITION),

		_float3(2.f, 2.f, 2.f),

		_float3(0.f, 1.f, 0.f),

		1.f
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Stone_Stand"), &Desc)))
		return;
}

void CBoss_Nero::Skill_BulletCurtain()
{
	CBullet_Sphere::BULLET_DESC Desc = {};
	
	for (int i = 0; i < 24; i++)
	{
		Desc = { 
			_float3(50.f, 1.f, 50.f), 
			_float3(cos(D3DXToRadian(m_fDegreeAngle)), 0.f, sin(D3DXToRadian(m_fDegreeAngle))),
		};

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
			TEXT("Prototype_GameObject_Bullet_Sphere"), &Desc)))
			return;

		m_fDegreeAngle += 15.f;
	}
}

void CBoss_Nero::Skill_BulletShoot()
{
	CBullet_Sphere::BULLET_DESC Desc = {};

	Desc = {
		m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_RIGHT),
		m_pTransformPlayer->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Bullet_Sphere"), &Desc)))
		return;

	Desc = {
		m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_RIGHT),
		m_pTransformPlayer->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Bullet_Sphere"), &Desc)))
		return;
}

void CBoss_Nero::Skill_Beam() // 원기둥 기울이기가 가능한가?
{
	CBeam_Cylinder::BULLET_DESC Desc = {};

	Desc = {
		m_pTransformCom->Get_State(CTransform::STATE_POSITION),
		m_pTransformPlayer->Get_State(CTransform::STATE_POSITION)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
	TEXT("Prototype_GameObject_Beam_Cylinder"), &Desc)))
	return;
}

void CBoss_Nero::Skill_WallDash()
{
	CBullet_Cube::BULLET_DESC Desc = {};

	Desc = {
		_float3(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x
			, 4.2f
			, m_pTransformCom->Get_State(CTransform::STATE_POSITION).z),

		_float3(m_vDeltaPlayer.x
			, 0.f
			, m_vDeltaPlayer.z),

		_float3(m_pTransformPlayer->Get_State(CTransform::STATE_POSITION).x
			, 4.2f
			, m_pTransformPlayer->Get_State(CTransform::STATE_POSITION).z)		
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Bullet_Cube"), &Desc)))
		return;
}

void CBoss_Nero::Skill_IceDrop()
{
	CBullet_Corn::BULLET_DESC Desc = {};

	Desc = {
		m_pTransformPlayer->Get_State(CTransform::STATE_POSITION),
		_float3(0.f, -1.f, 0.f)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_WarningSign_Icedrop"), &Desc)))
		return;

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
	//	TEXT("Prototype_GameObject_Bullet_Corn"), &Desc)))
	//	return;
}

void CBoss_Nero::Skill_Pull() // 사각뿔 기울이기가 가능한가
{
	CPull_Pyramid::BULLET_DESC Desc = {};

	Desc = {
		m_pTransformCom->Get_State(CTransform::STATE_POSITION),
		m_pTransformPlayer->Get_State(CTransform::STATE_POSITION),
		m_pTransformPlayer
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Pull_Pyramid"), &Desc)))
		return;
}

void CBoss_Nero::Skill_RazorLine()
{
	CRazor_Cylinder::BULLET_DESC Desc = {};

	Desc = {
		_float3 (0.f, 0.5f, 50.f),
		_float3 (0.f, 0.f, 1.f),
		_float3 (0.f, 1.f, 0.f),
		_float3 (1.f, 0.f, 0.f)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Razor_Cylinder"), &Desc)))
		return;

	Desc = {
		_float3 (50.f, 0.5f, 100.f),
		_float3 (1.f, 0.f, 0.f),
		_float3 (0.f, 1.f, 0.f),
		_float3 (0.f, 0.f, -1.f)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Razor_Cylinder"), &Desc)))
		return;

	Desc = {
		_float3 (100.f, 0.5f, 50.f),
		_float3 (0.f, 0.f, -1.f),
		_float3 (0.f, 1.f, 0.f),
		_float3 (-1.f, 0.f, 0.f)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Razor_Cylinder"), &Desc)))
		return;

	Desc = {
		_float3 (50.f, 0.5f, 0.f),
		_float3 (-1.f, 0.f, 0.f),
		_float3 (0.f, 1.f, 0.f),
		_float3 (0.f, 0.f, 1.f)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Bullet"),
		TEXT("Prototype_GameObject_Razor_Cylinder"), &Desc)))
		return;
}

void CBoss_Nero::Skill_ClockHand()
{
	CBar_Cylinder::BULLET_DESC Desc = {};

	Desc = {
		m_pTransformPlayer->Get_State(CTransform::STATE_POSITION),
		_float3(0.f, 0.f ,1.f)
	};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_WarningSign_BarCylinder"), &Desc)))
		return;
}

void CBoss_Nero::Effect_Before_Beam()
{
	CEffect::EFFECT_DESC Desc = {TEXT("Prototype_Component_Texture_Explosion9"), 0.3f};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Effect"), 
		TEXT("Prototype_GameObject_Effect"), &Desc)))
		MSG_BOX(TEXT("NO EFFECT"));

	CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back());
	
	pEffect->Set_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION)); // 네로 위치에서 생성
	pEffect->Scaling(10.f, 10.f, 1.f);

	m_dwEffectTime = GetTickCount();
}

void CBoss_Nero::Effect_Before_Pull()
{
	CEffect::EFFECT_DESC Desc = { TEXT("Prototype_Component_Texture_Explosion5"), 0.3f };

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eCurLevel, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Effect"), &Desc)))
		MSG_BOX(TEXT("NO EFFECT"));

	CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back());

	pEffect->Set_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION)); // 네로 위치에서 생성
	pEffect->Scaling(5.f, 5.f, 1.f);

	m_dwEffectTime = GetTickCount();
}

CBoss_Nero* CBoss_Nero::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss_Nero* pInstance = new CBoss_Nero(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("failed to Create Prototype Nero"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Nero::Clone(void* pArg)
{
	CBoss_Nero* pInstance = new CBoss_Nero(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("failed to Clone Nero"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Nero::Free()
{

	__super::Free();

}
