#include "stdafx.h"
#include "..\Public\ChaosPatternB.h"

#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Bullet.h"

CChaosPatternB::CChaosPatternB(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject { pGraphic_Device }
{
}

CChaosPatternB::CChaosPatternB(const CChaosPatternB & Prototype)
	: CLandObject { Prototype }
{
}


HRESULT CChaosPatternB::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChaosPatternB::Initialize(void * pArg)
{
	

	if (FAILED(Ready_Components()))
		return E_FAIL;


	PATTERN_DESC* pDesc = static_cast<PATTERN_DESC*>(pArg);
#pragma region Desc



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_vStartPos = pDesc->vPos;
	m_vArrivePos = pDesc->vTargetPos;
#pragma endregion


	m_pTransformCom->Scaling(5.f, 5.f, 1.f);
	m_fDirection = pDesc->vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float distance = D3DXVec3Length(&m_fDirection);


	// 거리 벡터의 수평 거리와 높이 분리
	float horizontalDistance = sqrt(m_fDirection.x * m_fDirection.x + m_fDirection.z * m_fDirection.z);
	float verticalDistance = m_fDirection.y;
	// 초기 속도 계산
	float angle = 40.f;
	float radians = D3DXToRadian(angle);
	float initialSpeed = sqrt((m_fGravity * horizontalDistance * horizontalDistance) /
		(2 * (horizontalDistance * tan(radians) - verticalDistance) * cos(radians) * cos(radians)));

	m_fVelocity.x = initialSpeed * (m_fDirection.x / horizontalDistance);
	m_fVelocity.y = initialSpeed * sin(radians);
	m_fVelocity.z = initialSpeed * (m_fDirection.z / horizontalDistance);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CChaosPatternB::Priority_Update(_float fTimeDelta)
{
}

int CChaosPatternB::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;

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

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);

	return OBJ_NOEVENT;
}

void CChaosPatternB::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);


	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_WALL, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		m_bDir = !m_bDir;
	}
	if (m_iHP <= 0)
	{

	}
}

HRESULT CChaosPatternB::Render()
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

void CChaosPatternB::Update_Spawn(_float fTimeDelta)
{
	m_pTextureCom[m_eState]->Move_Frame(fTimeDelta);
		
	m_fSumTime += fTimeDelta;

	_float3 CurrentPos;
	CurrentPos.x = m_vStartPos.x + m_fVelocity.x * m_fSumTime;
	CurrentPos.y = m_vStartPos.y + m_fVelocity.y * m_fSumTime - 0.5f * m_fGravity * m_fSumTime * m_fSumTime;
	CurrentPos.z = m_vStartPos.z + m_fVelocity.z * m_fSumTime;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CurrentPos);

	_float dist = D3DXVec3Length(&(m_vArrivePos - CurrentPos));

	if(dist<1.5f || CurrentPos.y <=1.7f)
		m_eState = PATTERN_IDLE;
}

void CChaosPatternB::Update_Idle(_float fTimeDelta)
{

	if(m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_CHARGE;
}

void CChaosPatternB::Update_Charge(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_DEATH;
}


void CChaosPatternB::Update_Death(_float fTimeDelta)
{

	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_bDead = true;
}


HRESULT CChaosPatternB::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 38;
	TextureDesc.fSpeed = 0.08f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternB_Spawn"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_SPAWN]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 62;
	TextureDesc.fSpeed = 0.01f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternB_Idle"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_IDLE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 17;
	TextureDesc.fSpeed = 0.005f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternB_Charge"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_CHARGE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 47;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_CHAOS, TEXT("Prototype_Component_Texture_Chaos_PatternB_Death"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_DEATH]), &TextureDesc)))
		return E_FAIL;




	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 5.0f;
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

HRESULT CChaosPatternB::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);


	return S_OK;
	
}

HRESULT CChaosPatternB::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}


CChaosPatternB * CChaosPatternB::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CChaosPatternB*		pInstance = new CChaosPatternB(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChaosPatternB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CChaosPatternB::Clone(void* pArg)
{
	CChaosPatternB*		pInstance = new CChaosPatternB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CChaosPatternB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaosPatternB::Free()
{
	__super::Free();

	for(int i = 0; i< PATTERN_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
