#include "stdafx.h"
#include "..\Public\FellPattern.h"

#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Bullet.h"

CFellPattern::CFellPattern(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CFellPattern::CFellPattern(const CFellPattern & Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CFellPattern::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFellPattern::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	PATTERN_DESC* pDesc = static_cast<PATTERN_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_pTransformCom->Scaling(5.f, 5.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CFellPattern::Priority_Update(_float fTimeDelta)
{
}

int CFellPattern::Update(_float fTimeDelta)
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
	case PATTERN_ATTACK:
		Update_Charge(fTimeDelta);
		break;
	case PATTERN_DEATH:
		Update_Death(fTimeDelta);
		break;
	default:
		break;
	}

	if(m_eState == PATTERN_ATTACK || m_eState == PATTERN_SPAWN)
		m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);

	return OBJ_NOEVENT;
}

void CFellPattern::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
}

HRESULT CFellPattern::Render()
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

void CFellPattern::Update_Spawn(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_IDLE;
		
}

void CFellPattern::Update_Idle(_float fTimeDelta)
{
	if(m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_ATTACK;
}

void CFellPattern::Update_Charge(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_eState = PATTERN_DEATH;
}


void CFellPattern::Update_Death(_float fTimeDelta)
{
	if (m_pTextureCom[m_eState]->Move_Frame(fTimeDelta, false))
		m_bDead = true;
}


HRESULT CFellPattern::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	//LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 28;
	TextureDesc.fSpeed = 0.08f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Pattern_Spawn"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_SPAWN]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 39;
	TextureDesc.fSpeed = 0.01f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Pattern_Idle"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_IDLE]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 29;
	TextureDesc.fSpeed = 0.05f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Pattern_Attack"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[PATTERN_ATTACK]), &TextureDesc)))
		return E_FAIL;

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 24;
	TextureDesc.fSpeed = 0.02f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Fell_Pattern_Death"),
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

HRESULT CFellPattern::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);


	return S_OK;
	
}

HRESULT CFellPattern::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}


CFellPattern * CFellPattern::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFellPattern*		pInstance = new CFellPattern(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFellPattern"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CFellPattern::Clone(void* pArg)
{
	CFellPattern*		pInstance = new CFellPattern(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CFellPattern"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFellPattern::Free()
{
	__super::Free();

	for(int i = 0; i< PATTERN_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
