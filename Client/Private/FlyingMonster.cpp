#include "stdafx.h"
#include "..\Public\FlyingMonster.h"

#include "GameInstance.h"
#include "Effect.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h" //에너미 desc 쓰려고 부른거임! 상속아님!


CFlyingMonster::CFlyingMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEnemy{ pGraphic_Device }
{
}

CFlyingMonster::CFlyingMonster(const CFlyingMonster & Prototype)
	: CEnemy{ Prototype }
{
}


HRESULT CFlyingMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlyingMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CEnemy::ENEMY_DESC* desc = static_cast<CEnemy::ENEMY_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, desc->vDir);
	m_pTransformCom->Scaling(3.f, 3.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CFlyingMonster::Priority_Update(_float fTimeDelta)
{
	if(!m_bDead)
		m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
}

int CFlyingMonster::Update(_float fTimeDelta)
{
	if (this->m_bDead)
	{
		CEffect::EFFECT_DESC effDesc;
		effDesc.iFrameSpeed = 0.3f;
		effDesc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion7");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &effDesc)))
			MSG_BOX(TEXT("NO EFFECT"));

		m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(Get_Pos());
		return OBJ_DEAD;
	}

	m_fSumTime += fTimeDelta;

	switch (m_eState)
	{
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

	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);

	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);

	__super::SetUp_OnTerrain(m_pTransformCom, 5.f);

	return OBJ_NOEVENT;
}

void CFlyingMonster::Late_Update(_float fTimeDelta)
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
		m_eState = STATE_DEATH;
	}
	float scrollSpeed = 0.5f; // 텍스처 스크롤 속도
	scrollOffset += scrollSpeed * fTimeDelta; // elapsedTime은 프레임 시간(초)입니다.

	// 텍스처 평행 이동 행렬 생성
	if (scrollOffset >= 1.0f)
		scrollOffset -= 1.0f;


	// 텍스처 변환 행렬 설정
	D3DXMatrixRotationX(&matTex, scrollOffset);
	m_pGraphic_Device->SetTransform(D3DTS_TEXTURE0, &matTex);
}

HRESULT CFlyingMonster::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	m_pGraphic_Device->SetTransform(D3DTS_TEXTURE0, &matTex);

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

void CFlyingMonster::Update_Idle(_float fTimeDelta)
{
	LEVELID eLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
	CGameObject* pPlayer = m_pGameInstance->Find_Object(eLevel, TEXT("Layer_Player"));
	
	if (!pPlayer)
		return;

	_float3 dir = pPlayer->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (D3DXVec3Length(&dir) > 15.f)
		m_pTransformCom->Go_PosDir(fTimeDelta, dir);
	else
		m_eState = STATE_ATTACK;


}

void CFlyingMonster::Update_Attack(_float fTimeDelta)
{
	LEVELID eLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
	CGameObject* pPlayer = m_pGameInstance->Find_Object(eLevel, TEXT("Layer_Player"));

	if (!pPlayer)
		return;

	_float3 dir = pPlayer->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (D3DXVec3Length(&dir) > 20.f)

		m_eState = STATE_IDLE;

	if(!m_bDir)
		m_pTransformCom->Go_Left(fTimeDelta);
	else
		m_pTransformCom->Go_Right(fTimeDelta);


	if (m_fSumTime <= m_fAttackTime)
		return;

	m_fSumTime = 0.f;

	CBullet::BulletDesc	Desc = {};
	Desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Desc.vLook = *D3DXVec3Normalize(&dir, &dir);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Bullet"), &Desc)))
	{
		MSG_BOX(TEXT("총알 생성 실패"));
	}

}

void CFlyingMonster::Update_Death(_float fTimeDelta)
{
	if (!m_bFistDead)
	{
		m_bFistDead = true;
		m_pGameInstance->PlayDefault(TEXT("FlyingMonster_Dead.wav"), 25, 0.8f, false);
	}
	if ((m_pTextureCom[m_eState]->Get_Frame().iCurrentTex+1) >= m_pTextureCom[m_eState]->Get_Frame().iEndTex)
		m_bDead = true;

}

void CFlyingMonster::IsPicked(_float3* pOutPos, _float* pDist)
{
	if (m_bDead == true|| m_eState == STATE_DEATH)
		return;


	_float3 vHitingPos;
	D3DXVec3TransformCoord(&vHitingPos, pOutPos, &m_pTransformCom->Get_WorldMatrix());

	CEffect::EFFECT_DESC Desc;

	CPlayer* pPlayer = nullptr;

	pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

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

	if (m_iHP <= 0)
		m_eState = STATE_DEATH;

	pPlayer->Plus_Fever(0.1f);


}

HRESULT CFlyingMonster::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 23;
	TextureDesc.fSpeed = 0.08f;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlyingMonster_Idle"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[STATE_IDLE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 23;
	TextureDesc.fSpeed = 0.08f;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlyingMonster_Attack"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[STATE_ATTACK]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 15;
	TextureDesc.fSpeed = 0.03;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlyingMonster_Death"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[STATE_DEATH]), &TextureDesc)))
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

HRESULT CFlyingMonster::SetUp_RenderState()
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

HRESULT CFlyingMonster::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);



	return S_OK;
}


CFlyingMonster * CFlyingMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFlyingMonster*		pInstance = new CFlyingMonster(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFlyingMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CFlyingMonster::Clone(void* pArg)
{
	CFlyingMonster*		pInstance = new CFlyingMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CFlyingMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlyingMonster::Free()
{
	__super::Free();

	for(int i = 0; i< STATE_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
