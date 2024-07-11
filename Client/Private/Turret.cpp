#include "stdafx.h"
#include "..\Public\Turret.h"

#include "GameInstance.h"
#include "Bullet.h"
#include "Effect.h"
#include "Player.h"

CTurret::CTurret(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CTurret::CTurret(const CTurret & Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CTurret::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTurret::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(10.f, 2.45f, 10.f));
	m_pTransformCom->Scaling(8.f, 8.f, 1.f);


	return S_OK;
}

void CTurret::Priority_Update(_float fTimeDelta)
{
	if (m_eState != STATE_DEATH && m_bDead == false)
		m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
}

int CTurret::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;

	m_fSumTime += fTimeDelta;

	switch (m_eState)
	{
	case STATE_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case STATE_IDLE:
		Update_Idle(fTimeDelta);
		break;
	case STATE_ATTACK:
		Update_Attack(fTimeDelta);
		break;
	case STATE_DEATH:
		Update_Death(fTimeDelta);
		break;
	}



	return OBJ_NOEVENT;
}

void CTurret::Late_Update(_float fTimeDelta)
{
	if (m_iHP <= 0)
	{
		m_eState = STATE_DEATH;
	}

	m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);


}

HRESULT CTurret::Render()
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

void CTurret::Update_Spawn(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = STATE_IDLE;
}

void CTurret::Update_Idle(_float fTimeDelta)
{
	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);

	LEVELID eLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
	CGameObject* pPlayer = m_pGameInstance->Find_Object(eLevel, TEXT("Layer_Player"));
	
	if (!pPlayer)
		return;

	if (m_fSumTime <= 1.0f)
		return;

	m_fSumTime = 0.f;

	m_vAttackDir = pPlayer->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);



	_float3 dir = pPlayer->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (D3DXVec3Length(&dir) > 120.f)
		return;
	else
	{
		m_eState = STATE_ATTACK;
	}
}


void CTurret::Update_Attack(_float fTimeDelta)
{
	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);

	if (m_fSumTime <= m_fAttackTime)
		return;

	m_fSumTime = 0.f;

	CBullet::BulletDesc	Desc = {};
	Desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Desc.vLook = *D3DXVec3Normalize(&m_vAttackDir, &m_vAttackDir);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Bullet"), &Desc)))
	{
		MSG_BOX(TEXT("총알 생성 실패"));
	}
	m_iBulletCount++;

	if (m_iBulletCount > 8)
	{
		m_iBulletCount = 0;
		m_eState = STATE_IDLE;
	}
}

void CTurret::Update_Death(_float fTimeDelta)
{
	if (m_bDeadStart)
	{
		m_pGameInstance->PlayDefault(TEXT("Turret_Dead.wav"), 30, 0.8f, false);
		m_bDeadStart = false;
	}
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
	{
		m_bDead = true;
	}


}

void CTurret::IsPicked(_float3* pOutPos, _float* pDist)
{
	if (m_bDead || m_eState == STATE_DEATH)
		return;

	_float2 vPos = _float2(pOutPos->x, pOutPos->y);
	if (D3DXVec2Length(&vPos) >= 0.25f)
		return;

	_float3 vHitingPos;
	D3DXVec3TransformCoord(&vHitingPos, pOutPos, &m_pTransformCom->Get_WorldMatrix());

	CPlayer* pPlayer = nullptr;

	pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	if (pPlayer == nullptr)
		return;

	CEffect::EFFECT_DESC Desc;
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
	pPlayer->Plus_Fever(0.1f);


}

HRESULT CTurret::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 28;
	TextureDesc.fSpeed = 0.05f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Turret_Spawn"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[STATE_SPAWN]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 60;
	TextureDesc.fSpeed = 0.05f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Turret_Idle"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[STATE_IDLE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 38;
	TextureDesc.fSpeed = 0.1f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Turret_Attack"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[STATE_ATTACK]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 29;
	TextureDesc.fSpeed = 0.01f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Turret_Dead"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTextureCom[STATE_DEATH]), &TextureDesc)))
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

HRESULT CTurret::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100);


	return S_OK;
	
}

HRESULT CTurret::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);



	return S_OK;
}


CTurret * CTurret::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTurret*		pInstance = new CTurret(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTurret"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CTurret::Clone(void* pArg)
{
	CTurret*		pInstance = new CTurret(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTurret"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTurret::Free()
{
	__super::Free();

	for (int i = 0; i < STATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}
		
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
