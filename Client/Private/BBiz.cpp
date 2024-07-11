#include "stdafx.h"
#include "..\Public\BBiz.h"

#include "GameInstance.h"
#include "Interection.h"
#include "UIScript.h"

CBBiz::CBBiz(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CBBiz::CBBiz(const CBBiz & Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CBBiz::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBBiz::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;
	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(50.f, 3.f, 82.f));
	m_pTransformCom->Scaling(2.f, 2.f, 1.f);
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

void CBBiz::Priority_Update(_float fTimeDelta)
{
}

int CBBiz::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_pTransformCom->Set_Billboard();


	return OBJ_NOEVENT;
}

void CBBiz::Late_Update(_float fTimeDelta)
{
	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		static_cast<CInterection*>(m_pInterection)->Set_RenderState(true); // 상호작용키 렌더적용
		if (m_pGameInstance->GetButtonDown(KeyType::E)) //E 키를 누르면 죽게함
		{
			if(FAILED(AddScript()))
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

HRESULT CBBiz::Render()
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



HRESULT CBBiz::Ready_Components()
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	
	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 16;
	TextureDesc.fSpeed = 0.1f;

	if (FAILED(__super::Add_Component(LEVEL_PALACE, TEXT("Prototype_Component_Texture_BBiz"),
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
	CollCubeDesc.fRadiusY = 2.f;
	CollCubeDesc.fRadiusX = 3.f;
	CollCubeDesc.fRadiusZ = 8.f;
	CollCubeDesc.fOffSetX = -2.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBBiz::SetUp_RenderState()
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

HRESULT CBBiz::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CBBiz::AddScript()
{
	{
		CUIScript::SCRIPT_DESC Desc;
		Desc.eLevel = LEVEL_PALACE;
		Desc.iTalker = 2;
		Desc.Script = TEXT("현자 : 당신이 가토군.. 내 부탁좀 들어주시게");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_PALACE, TEXT("Layer_Script"),
			TEXT("Prototype_GameObject_UIScript"), &Desc)))
			return E_FAIL;
	}
	{
		CUIScript::SCRIPT_DESC Desc;
		Desc.eLevel = LEVEL_PALACE;
		Desc.iTalker = 0;
		Desc.Script = TEXT("가토 : 무슨 부탁이지?");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_PALACE, TEXT("Layer_Script"),
			TEXT("Prototype_GameObject_UIScript"), &Desc)))
			return E_FAIL;
	}

	{
		CUIScript::SCRIPT_DESC Desc;
		Desc.eLevel = LEVEL_PALACE;
		Desc.iTalker = 2;
		Desc.Script = TEXT("현자 : 우리행성 지하 미궁에 괴물들과\n반역자 네로가 깨어났다네\n그들을 처치해주게");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_PALACE, TEXT("Layer_Script"),
			TEXT("Prototype_GameObject_UIScript"), &Desc)))
			return E_FAIL;
	}

	{
		CUIScript::SCRIPT_DESC Desc;
		Desc.eLevel = LEVEL_PALACE;
		Desc.iTalker = 2;
		Desc.Script = TEXT("현자 : 당신에게 도움이 될만한 펫을 하나 \n붙여주지 지하 미궁에 가면  도움을\n받을 수 있을걸세");
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_PALACE, TEXT("Layer_Script"),
			TEXT("Prototype_GameObject_UIScript"), &Desc)))
			return E_FAIL;
	}
	return S_OK;
}


CBBiz * CBBiz::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBBiz*		pInstance = new CBBiz(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBBiz"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CBBiz::Clone(void* pArg)
{
	CBBiz*		pInstance = new CBBiz(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBBiz"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBBiz::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

}
