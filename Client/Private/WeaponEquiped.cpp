#include "stdafx.h"
#include "WeaponEquiped.h"
#include "FSM.h"

#include "GameInstance.h"
#include "GunEffect.h"
#include "ParticleSystem.h"
#include "Player.h"
#include "Camera.h"

WeaponEquiped::WeaponEquiped(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

WeaponEquiped::WeaponEquiped(const WeaponEquiped& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT WeaponEquiped::Initialize_Prototype()
{
	m_pFSM = nullptr;
	return S_OK;
}

HRESULT WeaponEquiped::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fX = 376.f;
	m_fY = 583.f;
	m_fZ = 0.2f;

	m_fSizeX = 200.f;
	m_fSizeY = 144.f;

	__super::Set_OrthoLH();


	return S_OK;
}

void WeaponEquiped::Priority_Update(_float fTimeDelta)
{
}

int WeaponEquiped::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_NOEVENT;
	return OBJ_NOEVENT;
}

void WeaponEquiped::Late_Update(_float fTimeDelta)
{
	m_pFSM->Late_Update(fTimeDelta);

	m_fShootDegree -= 0.3f;
	m_fShootDegree = max(0.f, m_fShootDegree);
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian(m_fShootDegree));
}

HRESULT WeaponEquiped::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pFSM->Get_CurTextureCom()->Bind_Texture(m_pFSM->Get_CurTextureCom()->Get_Frame().iCurrentTex)))
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

void WeaponEquiped::Set_State(CFSM::OBJSTATE _eState)
{
	m_pFSM->Set_State(_eState);
}

HRESULT WeaponEquiped::Add_RenderObject(CGameObject* pRenderObject)
{
	return m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, pRenderObject);
}

void WeaponEquiped::Change_EquipingWeapon(int iWeaponNum)
{
	m_pFSM->Turn_Frame((WAY)iWeaponNum);
}

void WeaponEquiped::Shoot()
{
	m_fShootDegree = 7.f;
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian(m_fShootDegree));
	
}

HRESULT WeaponEquiped::Ready_Components()
{
	/* For.Com_Texture */
#pragma region 텍스쳐

	CTexture::TEXTUREDESC	TextureDesc{ 0 };

	TextureDesc = { 0, 0, 1, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponEquipped"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);

	TextureDesc = { 0, 0, 1, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponEquipped_Reinforce"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_WEAPONREINFORCE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_WEAPONREINFORCE, m_pTextureCom[CFSM::OBJSTATE_WEAPONREINFORCE]);

#pragma endregion

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

	return S_OK;
}

HRESULT WeaponEquiped::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);

	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_PrevProj);
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_PrevView);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT WeaponEquiped::Reset_RenderState()
{
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_PrevView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_PrevProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

HRESULT WeaponEquiped::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 1, 3 }; // 애니메이션 수 , 방향 수
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);
	Desc = { 1, 3 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_WEAPONREINFORCE, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

WeaponEquiped* WeaponEquiped::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	WeaponEquiped* pInstance = new WeaponEquiped(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeaponEquiped"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* WeaponEquiped::Clone(void* pArg)
{
	WeaponEquiped* pInstance = new WeaponEquiped(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CWeaponEquiped"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void WeaponEquiped::Free()
{
	__super::Free();


	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

	CFSM::Destroy(m_pFSM);
}
