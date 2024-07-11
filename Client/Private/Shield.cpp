#include "stdafx.h"
#include "Shield.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Vibuffer_Sphere.h"

CShield::CShield(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBlendObject{pGraphic_Device}
{

}

CShield::CShield(const CShield& Prototype)
	:CBlendObject{Prototype}
{
}

HRESULT CShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShield::Initialize(void* pArg)
{
	GRENADE_DESC Desc = *static_cast<GRENADE_DESC*>(pArg);
	m_pTransformPlayer = Desc.pTerrainTransform; //걍 구조체 재활용하려고 여따 넣은거다 ㅇㅋ?
	Safe_AddRef(m_pTransformPlayer);
	m_eCurLevel = Desc.eCurLevel;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	Ready_Components();
	return S_OK;
}

void CShield::Priority_Update(_float fTimeDelta)
{

}

int CShield::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;
	auto objects = m_pGameInstance->Get_ObjectList(m_eCurLevel, TEXT("Layer_Bullet"));
	if (nullptr != objects)
	{
		for (auto obj : *objects)
		{
			if (nullptr != obj)
			{
				if (D3DXVec3Length(&(obj->Get_Pos() - Get_Pos())) <= 1.8f)
					obj->KillSwitch();
			}
		}
	}
	return OBJ_NOEVENT;
}

void CShield::Late_Update(_float fTimeDelta)
{
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
	{
		MSG_BOX(TEXT("알파소팅 불가!"));
		return;
	}
	m_pTransformCom->GOTO(m_pTransformPlayer->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CShield::Render()
{
	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	_float4x4	ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CShield::KillSwitch()
{
	m_bDead = true;
}

HRESULT CShield::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HatField"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Sphere"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShield::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CShield::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

CShield* CShield::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShield* pInstance = new CShield(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShield::Clone(void* pArg)
{
	CShield* pInstance = new CShield(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShield::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformPlayer);
	Safe_Release(m_pVIBufferCom);

}
