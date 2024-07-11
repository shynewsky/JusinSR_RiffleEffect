#include "stdafx.h"
#include "Weapon.h"
#include "FSM.h"

#include "GameInstance.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	m_pFSM = nullptr;
	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pFSM->Turn_Frame((WAY)1);

	m_fX = 233.f;
	m_fY = 595.f;
	m_fZ = 0.12f;

	m_fSizeX = 300.f;
	m_fSizeY = 273.f;

	__super::Set_OrthoLH();

	return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
}

int CWeapon::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CWeapon::Late_Update(_float fTimeDelta)
{
	m_pFSM->Late_Update(fTimeDelta);
}

HRESULT CWeapon::Render()
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

void CWeapon::Set_State(CFSM::OBJSTATE _eState)
{
	m_pFSM->Set_State(_eState);
}

HRESULT CWeapon::Add_RenderObject(CGameObject* pRenderObject)
{
	return m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, pRenderObject);
}

void CWeapon::Change_Weapon(int iWeaponNum)
{
	m_pFSM->Turn_Frame((WAY)iWeaponNum);
}

HRESULT CWeapon::Ready_Components()
{
	/* For.Com_Texture */
#pragma region ÅØ½ºÃÄ

	CTexture::TEXTUREDESC	TextureDesc{ 0 };

	TextureDesc = { 0, 0, 4, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_WEAPON]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_WEAPON, m_pTextureCom[CFSM::OBJSTATE_WEAPON]);

	TextureDesc = { 0, 0, 4, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Reinforce"),
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

HRESULT CWeapon::SetUp_RenderState()
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

HRESULT CWeapon::Reset_RenderState()
{
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_PrevView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_PrevProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

HRESULT CWeapon::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 4, 3 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_WEAPON, Desc);
	Desc = { 4, 3 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_WEAPONREINFORCE, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_WEAPON);
	return S_OK;
}

CWeapon* CWeapon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWeapon* pInstance = new CWeapon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();


	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

	CFSM::Destroy(m_pFSM);
}
