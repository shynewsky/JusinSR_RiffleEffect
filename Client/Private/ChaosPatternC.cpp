#include "stdafx.h"
#include "..\Public\ChaosPatternC.h"

#include "GameInstance.h"
#include "Effect.h"
#include "Bullet.h"

CChaosPatternC::CChaosPatternC(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject { pGraphic_Device }
{
}

CChaosPatternC::CChaosPatternC(const CChaosPatternC & Prototype)
	: CLandObject { Prototype }
{
}


HRESULT CChaosPatternC::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChaosPatternC::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_CHAOS, TEXT("Layer_Player"));

	m_pTransformCom->Scaling(5.f, 5.f, 1.f);

	_float3 fSpawnPos = pPlayer->Get_Pos();
	fSpawnPos.y += 1.3f;
	Set_Pos(fSpawnPos);
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pGameInstance->Play3D(TEXT("PatternC_Spawn.wav"), 26, 0.8f, fSpawnPos, false);

	return S_OK;
}

void CChaosPatternC::Priority_Update(_float fTimeDelta)
{
	if(!m_bDead)
		m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
}

int CChaosPatternC::Update(_float fTimeDelta)
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
	case PATTERN_EXPLOSION:
		Update_Explosion(fTimeDelta);
		break;
	case PATTERN_DEATH:
		Update_Death(fTimeDelta);
		break;
	default:
		break;
	}

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);


	return OBJ_NOEVENT;
}

void CChaosPatternC::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	_float3 vDistance = _float3(0, 0, 0);

	if (m_iHP <= 0)
	{
		m_eState = PATTERN_DEATH;
		
	}
}

HRESULT CChaosPatternC::Render()
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

void CChaosPatternC::Update_Spawn(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_IDLE;
}

void CChaosPatternC::Update_Idle(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_CHARGE;
}

void CChaosPatternC::Update_Charge(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_EXPLOSION;
}

void CChaosPatternC::Update_Explosion(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_bDead = true;
	}
	if (m_pTextureCom[m_eState]->Get_Frame().iCurrentTex == 8)
	{
		m_pGameInstance->PlayDefault(TEXT("PatternC_Explosion.wav"), 27, 0.8, false);
		for (int i = 0; i < 18; ++i)
		{
			_float angle = i * 20.f;

			_float3 vLook = _float3(cos(D3DXToRadian(angle)), 0.f, sin(D3DXToRadian(angle)));

			CBullet::BulletDesc	Desc = {};
			Desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			Desc.vPos.y -= 0.5f;
			Desc.vLook = *D3DXVec3Normalize(&vLook, &vLook);

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_CHAOS, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Bullet"), &Desc)))
			{
				MSG_BOX(TEXT("총알 생성 실패"));
			}
		}
	}
		
}


void CChaosPatternC::Update_Death(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_pGameInstance->Play3D(TEXT("PatternC_Dead.wav"), 28, 1.0f, __super::Get_Pos(), false);
		m_bDead = true;
	}
}

void CChaosPatternC::IsPicked(_float3* pOutPos, _float* pDist)
{
	if (m_bDead)
		return;

	_float2 vPos = _float2(pOutPos->x, pOutPos->y);
	if (D3DXVec2Length(&vPos) >= 0.35f)
		return;

	_float3 vHitingPos;
	D3DXVec3TransformCoord(&vHitingPos, pOutPos, &m_pTransformCom->Get_WorldMatrix());

	CEffect::EFFECT_DESC Desc;
	Desc.iFrameSpeed = 0.3f;
	Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion3");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Desc)))
		MSG_BOX(TEXT("NO EFFECT"));

	m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(vHitingPos);
	
	m_iHP--;
}

HRESULT CChaosPatternC::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 30;
	TextureDesc.fSpeed = 0.04f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Spawn"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_SPAWN]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 53;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Idle"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_IDLE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 44;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Charge"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_CHARGE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 37;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Explosion"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_EXPLOSION]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 49;
	TextureDesc.fSpeed = 0.01f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternC_Death"),
		TEXT("Com_Texture4"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_DEATH]), &TextureDesc)))
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


	return S_OK;
}

HRESULT CChaosPatternC::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);


	return S_OK;
	
}

HRESULT CChaosPatternC::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}


CChaosPatternC * CChaosPatternC::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CChaosPatternC*		pInstance = new CChaosPatternC(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChaosPatternC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CChaosPatternC::Clone(void* pArg)
{
	CChaosPatternC*		pInstance = new CChaosPatternC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CChaosPatternC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaosPatternC::Free()
{
	__super::Free();

	for(int i = 0; i< PATTERN_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

}
