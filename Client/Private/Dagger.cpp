#include "stdafx.h"
#include "Dagger.h"
#include "GameInstance.h"



CDagger::CDagger(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CEnemy_SM{ pGraphic_Device }
{
}

CDagger::CDagger(const CDagger& Prototype)
	:CEnemy_SM{ Prototype }
{
}

HRESULT CDagger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDagger::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	EnemySM_Desc* desc = static_cast<EnemySM_Desc*>(pArg);

	m_pTransformCom->GOTO(desc->vPos);

	m_pTransformCom->Scaling(2.f, 2.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CDagger::Priority_Update(_float fTimeDelta)
{

}

int CDagger::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;
	_float Chrono = m_pGameInstance->Get_CurSpeed();

	m_pTextureCom[0]->Set_FrameSpeed(0.1f / Chrono);

	m_pTransformCom->Move(m_bLeft ? _float3{ 4.f * fTimeDelta * Chrono, 0.f, 0.f } : _float3{ -4.f * fTimeDelta * Chrono, 0.f, 0.f });

	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BULLET, this);

	return OBJ_NOEVENT;
}

void CDagger::Late_Update(_float fTimeDelta)
{
	//SetUp_OnTerrain(m_pTransformCom, 0.5f);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	m_pTextureCom[0]->Move_Frame(fTimeDelta, true);


	_float3 vPos = Get_Pos();
	if (vPos.x <= 49.f || vPos.x >= 91.f)
	{
		m_bDead = true;
	}
	m_pTransformCom->Set_Billboard();

}

HRESULT CDagger::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom[0]->Bind_Texture(m_pTextureCom[0]->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	_float4x4		ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	//m_pColliderCom->Render_ColliderBox();

	return S_OK;
}

HRESULT CDagger::Ready_Components()
{
	//texture
	CTexture::TEXTUREDESC   TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	//Prototype_Component_Texture_WaveShot
	TextureDesc = { 0, 0, 4, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grenade"),
		TEXT("Com_Texture", &(int)CFSM::OBJSTATE_IDLE), reinterpret_cast<CComponent**>(&m_pTextureCom[0]), &TextureDesc)))
		return E_FAIL;

	//VI버퍼
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return S_OK;

	//트랜스폼
	CTransform::TRANSFORM_DESC  TransformDesc{};
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider_Cube*/

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 0.5f;
	CollCubeDesc.fRadiusX = 0.5f;
	CollCubeDesc.fRadiusZ = 0.2f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDagger::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);

	return S_OK;
}

HRESULT CDagger::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CDagger* CDagger::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDagger* pInstance = new CDagger(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CDagger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDagger::Clone(void* pArg)
{
	CDagger* pInstance = new CDagger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CDagger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDagger::Free()
{
	__super::Free();
}
