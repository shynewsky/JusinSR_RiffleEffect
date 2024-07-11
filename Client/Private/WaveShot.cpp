#include "stdafx.h"
#include "WaveShot.h"
#include "GameInstance.h"



CWaveShot::CWaveShot(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CEnemy_SM{ pGraphic_Device }
{
}

CWaveShot::CWaveShot(const CWaveShot& Prototype)
	:CEnemy_SM{ Prototype }
{
}

HRESULT CWaveShot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaveShot::Initialize(void* pArg)
{
	if (__super::Initialize(pArg))
		return E_FAIL;

	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	EnemySM_Desc* desc = static_cast<EnemySM_Desc*>(pArg);

	m_pTransformCom->GOTO(desc->vPos);

	m_pTransformCom->Scaling(1.f, 1.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CWaveShot::Priority_Update(_float fTimeDelta)
{

}

int CWaveShot::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	_float Chrono = m_pGameInstance->Get_CurSpeed();

	if (!m_bJumpDelay)
	{
		m_pTransformCom->Move(m_bLeft ? _float3{ 5.f * fTimeDelta * Chrono, 0.f, 0.f } : _float3{ -5.f * fTimeDelta * Chrono, 0.f, 0.f });
	}

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));

	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	return OBJ_NOEVENT;
}

void CWaveShot::Late_Update(_float fTimeDelta)
{
	SetUp_OnTerrain(m_pTransformCom, 0.5f);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	if (OBJ_DEAD == m_pFSM->Late_Update(fTimeDelta) && !m_bJumpDelay) //프레임 끝나면
	{
		m_bJumpDelay = true;
		m_fJumpDleay = 0.5f;
	}

	if (m_bJumpDelay == true && 0 >= m_fJumpDleay)
	{
		m_pFSM->Reset_Frame();
		m_bJumpDelay = false;
	}

	_float3 vPos = Get_Pos();
	if (vPos.x <= 0.f || vPos.x >= 100.f || vPos.z <= 0.f || vPos.z >= 100.f)
	{
		m_bDead = true;
	}
	m_pTransformCom->Set_Billboard();

	m_fJumpDleay -= fTimeDelta;

}

HRESULT CWaveShot::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom[m_pFSM->Get_CurState()]->Bind_Texture(m_pTextureCom[m_pFSM->Get_CurState()]->Get_Frame().iCurrentTex)))
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

	return S_OK;
}

HRESULT CWaveShot::Ready_Components()
{
	//texture
	CTexture::TEXTUREDESC   TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	//Prototype_Component_Texture_WaveShot
	TextureDesc = { 0, 0, 18, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_SM, TEXT("Prototype_Component_Texture_WaveShot"),
		TEXT("Com_Texture", &(int)CFSM::OBJSTATE_DIE), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_DIE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_DIE, m_pTextureCom[CFSM::OBJSTATE_DIE]);

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
	CollCubeDesc.fRadiusZ = 0.5f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWaveShot::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);

	return S_OK;
}

HRESULT CWaveShot::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CWaveShot::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC desc;
	desc = { 9,2 }; //9장 2방향
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_DIE, desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_DIE);

	return S_OK;
}

CWaveShot* CWaveShot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWaveShot* pInstance = new CWaveShot(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CWaveShot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWaveShot::Clone(void* pArg)
{
	CWaveShot* pInstance = new CWaveShot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CWaveShot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaveShot::Free()
{
	__super::Free();
}
