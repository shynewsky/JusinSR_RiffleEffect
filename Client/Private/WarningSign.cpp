#include "stdafx.h"
#include "WarningSign.h"
#include "FSM.h"

#include "GameInstance.h"

CWarningSign::CWarningSign(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderingObject{ pGraphic_Device }
{
}

CWarningSign::CWarningSign(const CWarningSign& Prototype)
	:CRenderingObject{ Prototype }
{
}

HRESULT CWarningSign::Initialize_Prototype()
{
	m_pFSM = nullptr;
	return S_OK;
}

HRESULT CWarningSign::Initialize(void* pArg)
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

void CWarningSign::Priority_Update(_float fTimeDelta) {}

int CWarningSign::Update(_float fTimeDelta)
{

	if (m_bDead)
	{
		m_pGameInstance->Play3D(TEXT("Flash_Active.wav"), 4, 0.1, m_pTransformCom->Get_State(CTransform::STATE_POSITION), false);

		if (m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_COLOSSEUM, TEXT("Layer_Corn"), TEXT("Prototype_GameObject_WormCorn")))
		{
			MSG_BOX(TEXT("Failed to Clone Corn"));
		}

		_float3 vSpawnPos = __super::Get_Pos();
		vSpawnPos.y -= 0.5;
		m_pGameInstance->Get_ObjectList(LEVEL_COLOSSEUM, TEXT("Layer_Corn"))->back()->Set_Pos(vSpawnPos);

		return OBJ_DEAD;
	}
	m_pTransformCom->Scaling(m_fCurTimer * 3, m_fCurTimer * 3, 0.f);
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
	{
		return OBJ_NOEVENT;
	}

	return OBJ_NOEVENT;
}

void CWarningSign::Late_Update(_float fTimeDelta)
{

	m_pFSM->Late_Update(fTimeDelta);
	m_fCurTimer += fTimeDelta;
	if (m_fCurTimer >= m_fTimer)
		m_bDead = true;
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);

}

HRESULT CWarningSign::Render()
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

HRESULT CWarningSign::Add_RenderObject(CGameObject* pRenderObject)
{
	return m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, pRenderObject);
}

HRESULT CWarningSign::Ready_Components()
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

HRESULT CWarningSign::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CWarningSign::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

HRESULT CWarningSign::SetUp_FSM()
{
	m_pFSM = CFSM::Create(nullptr);

	CFSM::Frame_DESC Desc;
	Desc = { 1,1 };
	m_pFSM->Add_FrameDesc(CFSM::OBJSTATE_IDLE, Desc);

	m_pFSM->Set_State(CFSM::OBJSTATE_IDLE);
	return S_OK;
}

CWarningSign* CWarningSign::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWarningSign* pInstance = new CWarningSign(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWarningSign"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWarningSign::Clone(void* pArg)
{
	CWarningSign* pInstance = new CWarningSign(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CWarningSign"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWarningSign::Free()
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
