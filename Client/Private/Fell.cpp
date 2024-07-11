#include "stdafx.h"
#include "..\Public\Fell.h"

#include "GameInstance.h"
#include "Effect.h"
#include "Bullet.h"
#include "Player.h"
#include "ParticleSystem.h"

#include "FellPattern.h"
#include "Fence.h"

CFell::CFell(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEnemy { pGraphic_Device }
{
}

CFell::CFell(const CFell & Prototype)
	: CEnemy{ Prototype }
{  

}


HRESULT CFell::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFell::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(117.f, 3.5f, 109.f));
	m_pTransformCom->Scaling(10.f, 10.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_iHP = 100;
	m_pGameInstance->PlayDefault(TEXT("Fell_Spawn.wav"), 26, 0.8f, false);

	return S_OK;
}

void CFell::Priority_Update(_float fTimeDelta)
{
	if(!m_bDead)
		m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
}

int CFell::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;

	m_fSumTime += fTimeDelta;

	switch (m_eState)
	{
	case FELL_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case FELL_IDLE:
		Update_Idle(fTimeDelta);
		break;
	case FELL_BURROW_START:
		Update_Burrow_Start(fTimeDelta);
		break;
	case FELL_BURROW:
		Update_Burrow(fTimeDelta);
		break;
	case FELL_BURROW_END:
		Update_Burrow_End(fTimeDelta);
		break;
	case FELL_DASH_START:
		Update_Dash_Start(fTimeDelta);
		break;
	case FELL_DASH_END:
		Update_Dash_End(fTimeDelta);
		break;
	case FELL_DEATH:
		Update_Death(fTimeDelta);
		break;
	default:
		break;
	}

	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);

	
	if (m_pGameInstance->GetButtonDown(KeyType::F1))
		m_eState = FELL_DEATH;
	if (m_pGameInstance->GetButtonDown(KeyType::Z))
		m_eState = FELL_BURROW_START;
	if (m_pGameInstance->GetButtonDown(KeyType::X))
		m_eState = FELL_BURROW;
	if (m_pGameInstance->GetButtonDown(KeyType::C))
		m_eState = FELL_BURROW_END;
	if (m_pGameInstance->GetButtonDown(KeyType::V))
		m_eState = FELL_DASH_START;
	if (m_pGameInstance->GetButtonDown(KeyType::B))
		m_eState = FELL_DASH_END;


	return OBJ_NOEVENT;
}

void CFell::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	//m_pTextureCom[CFSM::OBJSTATE_IDLE]->Move_Frame(fTimeDelta);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);


	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{

	}
	if (m_iHP <= 0)
	{
		m_eState = FELL_DEATH;
	}
}

HRESULT CFell::Render()
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

void CFell::Update_Spawn(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_eState = FELL_IDLE;
		m_fSumTime = 0.f;
	}
		
}

void CFell::Update_Idle(_float fTimeDelta)
{
	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);

	if (m_fSumTime < 2.0f)
	{
		return;
	}

	if (!m_bPattern)
	{
		m_eState = FELL_BURROW_START;
		m_bPattern = true;
	}
	else
	{
		m_eState = FELL_DASH_START;
		CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_MAZE, TEXT("Layer_Player"));
		if (!pPlayer)
			return;
		m_vDir = pPlayer->Get_Pos() - Get_Pos();
		m_vDir.y = 0.f;
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		m_fAngle = atan2f(m_vDir.x, m_vDir.z);
		m_bPattern = false;
	}
}

void CFell::Update_Burrow_Start(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_pTextureCom[m_eState]->Reset_Frame();
		m_eState = FELL_BURROW;
	}
}

void CFell::Update_Burrow(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_pGameInstance->StopSound(27);
		m_pGameInstance->PlayDefault(TEXT("Fell_Pattern.wav"), 27, 0.8f, false);
		CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_MAZE, TEXT("Layer_Player"));
		if (!pPlayer)
			return;
		m_vDir = pPlayer->Get_Pos() - Get_Pos();
		m_vDir.y = 0.f;

		D3DXVec3Normalize(&m_vDir, &m_vDir);


		_float4x4 matRotationLeft, matRotationRight;
		_float angle = D3DXToRadian(20); // 20도를 라디안으로 변환

		// y축을 기준으로 회전 (좌우 회전)
		D3DXMatrixRotationY(&matRotationLeft, -angle); // 좌측으로 20도 회전
		D3DXMatrixRotationY(&matRotationRight, angle); // 우측으로 20도 회전

		// 회전된 방향 벡터를 구합니다.

		D3DXVec3TransformNormal(&m_vDirLeft, &m_vDir, &matRotationLeft);
		D3DXVec3TransformNormal(&m_vDirRight, &m_vDir, &matRotationRight);

		m_pTextureCom[m_eState]->Reset_Frame();
		m_iBurrowCount++;
		CFellPattern::PATTERN_DESC Desc;
		for (int i = 1; i < 6; ++i)
		{
			Desc.vPos = Get_Pos() + (m_vDir * 3.5) * i;
			Desc.vPos.y = 1.5f;
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAZE, TEXT("Layer_FellPattern"), TEXT("Prototype_GameObject_FellPattern"), &Desc)))
				MSG_BOX(TEXT("NO Pattern"));

			Desc.vPos = Get_Pos() + (m_vDirLeft * 3.5) * i;
			Desc.vPos.y = 1.5f;
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAZE, TEXT("Layer_FellPattern"), TEXT("Prototype_GameObject_FellPattern"), &Desc)))
				MSG_BOX(TEXT("NO Pattern"));

			Desc.vPos = Get_Pos() + (m_vDirRight * 3.5) * i;
			Desc.vPos.y = 1.5f;
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAZE, TEXT("Layer_FellPattern"), TEXT("Prototype_GameObject_FellPattern"), &Desc)))
				MSG_BOX(TEXT("NO Pattern"));
		}
	}

	if (m_iBurrowCount > 3)
	{
		m_eState = FELL_BURROW_END;
		m_iBurrowCount = 0;
	}
		
}

void CFell::Update_Burrow_End(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_pTextureCom[m_eState]->Reset_Frame();
		m_eState = FELL_IDLE;
	}
		
}

void CFell::Update_Dash_Start(_float fTimeDelta)
{
	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);
	if (m_fSumTime > 0.1f)
	{
		m_fSumTime = 0.f;
		CBullet::BulletDesc	Desc = {};
		Desc.vPos = Get_Pos();
		Desc.vPos.y = 0.5f;

		_float3 vLook = _float3(cos(D3DXToRadian(m_fAngle + (m_iDashCount * 10))), 0.f, sin(D3DXToRadian(m_fAngle + (m_iDashCount * 10))));
		Desc.vLook = *D3DXVec3Normalize(&vLook, &vLook);

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Bullet"), &Desc)))
		{
			MSG_BOX(TEXT("총알 생성 실패"));
		}
		++m_iDashCount;
	}

	if (m_iDashCount > 72)
	{
		m_iDashCount = 0;
		m_eState = FELL_DASH_END;
	}
		

}

void CFell::Update_Dash_End(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = FELL_IDLE;
}


void CFell::Update_Death(_float fTimeDelta)
{
	if (m_bDeadSound)
	{
		m_bDeadSound = false;
		m_pGameInstance->PlayDefault(TEXT("Fell_Dead.wav"), 26, 0.8f, false);
	}
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_bDead = true;
}

void CFell::IsPicked(_float3* pOutPos, _float* pDist)
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
	EffectDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion13");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &EffectDesc)))
		MSG_BOX(TEXT("NO EFFECT"));

	m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(vHitingPos);

	pPlayer->Plus_Fever(0.1f);
}

HRESULT CFell::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 45;
	TextureDesc.fSpeed = 0.05f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Spawn"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[FELL_SPAWN]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 43;
	TextureDesc.fSpeed = 0.05f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Idle"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[FELL_IDLE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 19;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_BurrowStart"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[FELL_BURROW_START]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 13;
	TextureDesc.fSpeed = 0.09f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Burrow"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTextureCom[FELL_BURROW]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 27;
	TextureDesc.fSpeed = 0.04f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_BurrowEnd"),
		TEXT("Com_Texture8"), reinterpret_cast<CComponent**>(&m_pTextureCom[FELL_BURROW_END]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 30;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_DashStart"),
		TEXT("Com_Texture4"), reinterpret_cast<CComponent**>(&m_pTextureCom[FELL_DASH_START]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 38;
	TextureDesc.fSpeed = 0.01f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_DashEnd"),
		TEXT("Com_Texture5"), reinterpret_cast<CComponent**>(&m_pTextureCom[FELL_DASH_END]), &TextureDesc)))
		return E_FAIL;
		

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 48;
	TextureDesc.fSpeed = 0.05f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Death"),
		TEXT("Com_Texture6"), reinterpret_cast<CComponent**>(&m_pTextureCom[FELL_DEATH]), &TextureDesc)))
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
	CollCubeDesc.fRadiusX = 1.f;
	CollCubeDesc.fRadiusZ = 1.f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFell::SetUp_RenderState()
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

HRESULT CFell::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);



	return S_OK;
}


CFell * CFell::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFell*		pInstance = new CFell(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CFell::Clone(void* pArg)
{
	CFell*		pInstance = new CFell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CFell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFell::Free()
{
	__super::Free();

	for(int i = 0; i< FELL_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
