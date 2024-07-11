#include "stdafx.h"
#include "WarningSign_BarCylinder.h"
#include "FSM.h"
#include "Bar_Cylinder.h"
#include "Player.h"

#include "GameInstance.h"

CWarningSign_BarCylinder::CWarningSign_BarCylinder(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

CWarningSign_BarCylinder::CWarningSign_BarCylinder(const CWarningSign_BarCylinder& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CWarningSign_BarCylinder::Initialize_Prototype()
{
	m_pFSM = nullptr;
	return S_OK;
}

HRESULT CWarningSign_BarCylinder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	SetUp_FSM();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Move(_float3{0.f, 0.001f, 0.f});
	m_pTransformCom->GOTO(static_cast<RENDERINGOBJECT_DESC*>(pArg)->vPos);
	m_pTransformCom->Rotation({1.f, 0.f, 0.f}, D3DXToRadian(90.f));

	return S_OK;
}

void CWarningSign_BarCylinder::Priority_Update(_float fTimeDelta) {}

int CWarningSign_BarCylinder::Update(_float fTimeDelta)
{

	if (m_bDead)
	{
		m_pGameInstance->Play3D(TEXT("Flash_Active.wav"), 4, 0.1, m_pTransformCom->Get_State(CTransform::STATE_POSITION), false);

		CBar_Cylinder::BULLET_DESC Desc = {};

		Desc = {
			Get_Pos(),
			_float3(0.f, 0.f ,1.f)
		};
		if (m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_NERO, TEXT("Layer_Corn"), TEXT("Prototype_GameObject_Bar_Cylinder"), &Desc))
		{
			MSG_BOX(TEXT("Failed to Clone Bar_Cylinder"));
		}

		_float3 vSpawnPos = __super::Get_Pos();
		vSpawnPos.y -= 0.5;
		m_pGameInstance->Get_ObjectList(LEVEL_NERO, TEXT("Layer_Corn"))->back()->Set_Pos(vSpawnPos);

		CGameObject* pPlayer = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"))->front();
		if (D3DXVec3Length(&(Get_Pos() - pPlayer->Get_Pos())) < m_fTimer * 3)
		{
			static_cast<CPlayer*>(pPlayer)->HitPlayer(1);
		}

		return OBJ_DEAD;
	}
	m_pTransformCom->Scaling(m_fCurTimer * 3, m_fCurTimer * 3, 0.f);
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
	{
		return OBJ_NOEVENT;
	}

	return OBJ_NOEVENT;
}

void CWarningSign_BarCylinder::Late_Update(_float fTimeDelta)
{

	m_pFSM->Late_Update(fTimeDelta);
	m_fCurTimer += fTimeDelta;
	if (m_fCurTimer >= m_fTimer)
		m_bDead = true;
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);

}

HRESULT CWarningSign_BarCylinder::Render()
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

HRESULT CWarningSign_BarCylinder::Add_RenderObject(CGameObject* pRenderObject)
{
	return m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, pRenderObject);
}

HRESULT CWarningSign_BarCylinder::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC	TextureDesc{ 0 };

	TextureDesc = { 0, 0, 1, 0.1f };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WarningSign"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom[CFSM::OBJSTATE_IDLE]), &TextureDesc)))
		return E_FAIL;
	m_pFSM->Set_TextureCom(CFSM::OBJSTATE_IDLE, m_pTextureCom[CFSM::OBJSTATE_IDLE]);

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	//TransformDesc.fSpeedPerSec = 5.0f;
	//TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWarningSign_BarCylinder::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CWarningSign_BarCylinder::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

HRESULT CWarningSign_BarCylinder::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 1,1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

CWarningSign_BarCylinder* CWarningSign_BarCylinder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWarningSign_BarCylinder* pInstance = new CWarningSign_BarCylinder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWarningSign_BarCylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWarningSign_BarCylinder::Clone(void* pArg)
{
	CWarningSign_BarCylinder* pInstance = new CWarningSign_BarCylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CWarningSign_BarCylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWarningSign_BarCylinder::Free()
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
