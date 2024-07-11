#include "stdafx.h"
#include "..\Public\MapBox.h"

#include "GameInstance.h"
#include "Player.h"
#include "Camera.h"
#include "Effect.h"

CMapBox::CMapBox(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CMapBox::CMapBox(const CMapBox& Prototype)
	: CBlendObject{ Prototype }
{
}


HRESULT CMapBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapBox::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

void CMapBox::Priority_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_PickingGroup(CPicking_Manager::PICK_BULLET, this);
}

int CMapBox::Update(_float fTimeDelta)
{
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return OBJ_NOEVENT;

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	m_pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_WALL, this);

	return OBJ_NOEVENT;
}

void CMapBox::Late_Update(_float fTimeDelta)
{
	_float3 vDistance = _float3(0, 0, 0);
	if (m_pGameInstance->Collision_Check_Group(CCollider_Manager::COLLISION_PLAYER, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		if (pPlayer == nullptr)
			return;

		CTransform* pTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
		_float3 vPosition = pTransform->Get_State(CTransform::STATE_POSITION);


		if(m_pTransformCom->Get_State(CTransform::STATE_POSITION).y != 20.f)
		{
			if (fabsf(vDistance.x) < fabsf(vDistance.z))
				vPosition.x += vDistance.x;
			else
				vPosition.z += vDistance.z;

			pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}

	_float3 fOut;

	//백해드 카메라 일때만 박스에 가져지는지 판별
	CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera")));

	if (pCamera)
	{
		if (pCamera->Get_CurrentCameaMode() == CCamera::CAMERA_BACKHEAD)
		{
			_float3 fOut;
			if (m_pVIBufferCom->isBlinding(m_pTransformCom->Get_WorldMatrix(), &fOut))
			{
				m_eBlindState = BS_BLIND;
				m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
			}
			else
			{
				m_eBlindState = BS_NONE;
				m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

			}

		}
		else
			m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	}


}

HRESULT CMapBox::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	ZeroMemory(&m_pLight, sizeof(m_pLight));
	m_pGraphic_Device->GetLight(1, &m_pLight);  // 첫 번째 조명을 가져옵니다.
	D3DXVECTOR3 lightDir = -D3DXVECTOR3(m_pLight.Direction.x, m_pLight.Direction.y, m_pLight.Direction.z);
	D3DXVec3Normalize(&lightDir, &lightDir);
	m_pShaderCom->Set_RawValue("LightPosition", &D3DXVECTOR4(m_pLight.Position.x, m_pLight.Position.y, m_pLight.Position.z, 1.0f), sizeof(D3DXVECTOR4));
	m_pShaderCom->Set_RawValue("LightDiffuse", &D3DXVECTOR4(m_pLight.Diffuse.r, m_pLight.Diffuse.g, m_pLight.Diffuse.b, m_pLight.Diffuse.a), sizeof(D3DXVECTOR4));
	m_pShaderCom->Set_RawValue("LightAmbient", &D3DXVECTOR4(m_pLight.Ambient.r, m_pLight.Ambient.g, m_pLight.Ambient.b, m_pLight.Ambient.a), sizeof(D3DXVECTOR4));
	m_pShaderCom->Set_RawValue("LightRange", &_float(m_pLight.Range), sizeof(_float));
	m_pShaderCom->Set_RawValue("LightAttenuation", &_float(m_pLight.Attenuation1), sizeof(_float));

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_pTextureCom->Get_Frame().iCurrentTex));
	SetUp_RenderState();
	m_pShaderCom->Begin(m_eBlindState);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End();
	Reset_RenderState();

	return S_OK;
}

void CMapBox::IsPicked(_float3* pOutPos, _float* pDist)
{

	CEffect::EFFECT_DESC Desc;

	Desc.tFrameTextureTag = TEXT("Prototype_Component_Texture_Explosion2");

	Desc.iFrameSpeed = 0.3f;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Desc)))
		MSG_BOX(TEXT("NO EFFECT"));

	_float3 vPos = *pOutPos;
	m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"))->back()->Set_Pos(vPos);


}

HRESULT CMapBox::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

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

HRESULT CMapBox::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CMapBox::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CMapBox* CMapBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapBox* pInstance = new CMapBox(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMapBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMapBox::Clone(void* pArg)
{
	CMapBox* pInstance = new CMapBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMapBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
}
