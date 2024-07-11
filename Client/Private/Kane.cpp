#include "stdafx.h"
#include "..\Public\Kane.h"

#include "GameInstance.h"
#include "Interection.h"
#include "UIScript.h"

CKane::CKane(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CKane::CKane(const CKane & Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CKane::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKane::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;
	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(18.f, 0.5f, 15.f));
	m_pTransformCom->Scaling(1.f, 1.f, 1.f);
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	CUIOrthogonal::Orthogonal_DESC desc;

	desc.vPos = __super::Get_Pos();
	desc.fSizeX = 50.f;
	desc.fSizeY = 50.f;
	desc.bCull = false;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_PALACE, TEXT("Layer_Interection"), TEXT("Prototype_GameObject_Interection"), &desc)))
		return E_FAIL;

	if(m_pGameInstance->Get_ObjectList(LEVEL_PALACE, TEXT("Layer_Interection")) !=nullptr)
		m_pInterection = m_pGameInstance->Get_ObjectList(LEVEL_PALACE, TEXT("Layer_Interection"))->back();

	return S_OK;
}

void CKane::Priority_Update(_float fTimeDelta)
{
}

int CKane::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_pTransformCom->Set_Billboard();

	return OBJ_NOEVENT;
}

void CKane::Late_Update(_float fTimeDelta)
{
	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		static_cast<CInterection*>(m_pInterection)->Set_RenderState(true); // 상호작용키 렌더적용
		if (m_pGameInstance->GetButtonDown(KeyType::E)) //E 키를 누르면 죽게함
		{
			if (FAILED(AddScript()))
				return;

			m_pInterection->Set_Dead(true);
		}
	}
	else
	{
		if (m_pInterection) //충돌중이 아니라면 상호작용UI 끔
			static_cast<CInterection*>(m_pInterection)->Set_RenderState(false);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pTextureCom->Move_Frame(fTimeDelta);
}

HRESULT CKane::Render()
{	
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;		

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	//m_pColliderCom->Render_ColliderBox();

	return S_OK;
}



HRESULT CKane::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	
	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 16;
	TextureDesc.fSpeed = 0.1f;

	if (FAILED(__super::Add_Component(LEVEL_MAZE, TEXT("Prototype_Component_Texture_Kane"),
		TEXT("Com_Texture0", &(int)CFSM::OBJSTATE_IDLE), reinterpret_cast<CComponent**>(&m_pTextureCom), &TextureDesc)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	/* For.Com_Collider_Cube*/

	CCollider_Cube::COLLIDER_DESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CCollider_Cube::COLLIDER_DESC));
	CollCubeDesc.fRadiusY = 1.f;
	CollCubeDesc.fRadiusX = 1.f;
	CollCubeDesc.fRadiusZ = 1.f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CKane::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100);


	return S_OK;
	
}

HRESULT CKane::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CKane::AddScript()
{
	{
		CUIScript::SCRIPT_DESC Desc;
		Desc.eLevel = LEVEL_MAZE;
		Desc.iTalker = 3;
		Desc.Script = TEXT("케인 : 가토! 구하러 와줬구나!!");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAZE, TEXT("Layer_Script"),
			TEXT("Prototype_GameObject_UIScript"), &Desc)))
			return E_FAIL;
	}
	{
		CUIScript::SCRIPT_DESC Desc;
		Desc.eLevel = LEVEL_MAZE;
		Desc.iTalker = 0;
		Desc.Script = TEXT("가토 : 아니..펫 준다고 해서 왔는데...");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAZE, TEXT("Layer_Script"),
			TEXT("Prototype_GameObject_UIScript"), &Desc)))
			return E_FAIL;
	}

	{
		CUIScript::SCRIPT_DESC Desc;
		Desc.eLevel = LEVEL_MAZE;
		Desc.iTalker = 3;
		Desc.Script = TEXT("케인 : ...옜다 이자식아! 꼭 성공해!");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAZE, TEXT("Layer_Script"),
			TEXT("Prototype_GameObject_UIScript"), &Desc)))
			return E_FAIL;
	}
	return S_OK;
}


CKane * CKane::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CKane*		pInstance = new CKane(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKane"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CKane::Clone(void* pArg)
{
	CKane*		pInstance = new CKane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CKane"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKane::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
