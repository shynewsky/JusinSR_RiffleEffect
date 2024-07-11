#include "stdafx.h"
#include "WeaponLight.h"
#include "FSM.h"

#include "GameInstance.h"

CWeaponLight::CWeaponLight(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

CWeaponLight::CWeaponLight(const CWeaponLight& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CWeaponLight::Initialize_Prototype()
{
	m_pFSM = nullptr;
	return S_OK;
}

HRESULT CWeaponLight::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.4f, 0.4f, 0.4f);

	return S_OK;
}

void CWeaponLight::Priority_Update(_float fTimeDelta)
{
}

int CWeaponLight::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_NOEVENT;
	return OBJ_NOEVENT;
}

void CWeaponLight::Late_Update(_float fTimeDelta)
{
	m_pFSM->Late_Update(fTimeDelta);
	if (m_pTargetTransform == nullptr)
		return;

	m_pTransformCom->GOTO(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) +
		m_pTargetTransform->Get_State(CTransform::STATE_RIGHT) * -0.08f +
		_float3{0.f, 0.30f, 0.f} +
		m_pTargetTransform->Get_State(CTransform::STATE_LOOK) * 0.06f);

	m_pTransformCom->Set_Billboard();
}

HRESULT CWeaponLight::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pFSM->Get_CurTextureCom()->Bind_Texture(m_pFSM->Get_CurTextureCom()->Get_Frame().iCurrentTex)))
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

void CWeaponLight::Set_State(CFSM::OBJSTATE _eState)
{
	m_pFSM->Set_State(_eState);
}

HRESULT CWeaponLight::Add_RenderObject(CGameObject* pRenderObject)
{
	return m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, pRenderObject);
}

void CWeaponLight::Shoot(int iWeaponNum)
{
	m_pFSM->Set_State(CFSM::OBJSTATE_ROLL);
	
	m_pFSM->Turn_Frame((WAY)iWeaponNum);
}

HRESULT CWeaponLight::Ready_Components()
{
	/* For.Com_Texture */
#pragma region 텍스쳐

	CTexture::TEXTUREDESC	TextureDesc{ 0 };

	TextureDesc = { 0, 0, 4, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponLight"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_ROLL]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_ROLL, m_pTextureCom[CFSM::OBJSTATE_ROLL]);

	TextureDesc = { 0, 0, 1, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoRender"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);
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

HRESULT CWeaponLight::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CWeaponLight::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

HRESULT CWeaponLight::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 4, 3 }; // 애니메이션 수 , 방향 수
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_ROLL, Desc);

	Desc = { 1,1 };
	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

CWeaponLight* CWeaponLight::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWeaponLight* pInstance = new CWeaponLight(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeaponLight"));
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject* CWeaponLight::Clone(void* pArg)
{
	CWeaponLight* pInstance = new CWeaponLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CWeaponLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeaponLight::Free()
{
	__super::Free();


	for (int i = 0; i < CFSM::OBJSTATE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

	CFSM::Destroy(m_pFSM);
}
