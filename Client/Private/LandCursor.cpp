#include "stdafx.h"
#include "..\Public\LandCursor.h"

#include "GameInstance.h"
#include "Player.h"
#include "Bullet.h"
#include "Camera.h"


CLandCursor::CLandCursor(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{
}

CLandCursor::CLandCursor(const CLandCursor & Prototype)
	: CLandObject{ Prototype }
{
}


HRESULT CLandCursor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandCursor::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(2.f, 2.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(50.f, 3.f, 50.f));

	return S_OK;
}

void CLandCursor::Priority_Update(_float fTimeDelta)
{

}

int CLandCursor::Update(_float fTimeDelta)
{
	CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera")));

	if (!pCamera)
	{
		m_bRender = false;
		return OBJ_NOEVENT;
	}


	if (pCamera->Get_CurrentCameaMode() == CCamera::CAMERAMODE::CAMERA_FPS)
	{
		m_bRender = false;
		return OBJ_NOEVENT;
	}


	CTransform* pTransform = static_cast<CTransform*>(pCamera->Find_Component(TEXT("Com_Transform")));
	if (!pTransform)
	{
		m_bRender = false;
		return OBJ_NOEVENT;
	}
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	if (!pPlayer)
	{
		m_bRender = false;
		return OBJ_NOEVENT;
	}

	_float3 vCamLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCamLook.y = 0.f;
	_float3 vPos;
	D3DXVec3Normalize(&vPos, &vCamLook);

	_float3 vPlayerPos = pPlayer->Get_Pos();
	vPlayerPos += vPos * 5;
	Set_Pos(_float3(vPlayerPos.x, 0.5f, vPlayerPos.z));

	m_bRender = true;
	if (pCamera->Get_CurrentCameaMode() != CCamera::CAMERAMODE::CAMERA_BACKHEAD)
	{
		m_bRender = false;
	}
	m_pTransformCom->Scaling(2.f, 2.f, 1.f);
	m_pTextureCom->Move_Frame(fTimeDelta);
	__super::SetUp_OnTerrain(m_pTransformCom, 0.9f);
	m_pTransformCom->Set_Billboard();

	return OBJ_NOEVENT;
}

void CLandCursor::Late_Update(_float fTimeDelta)
{
	if(m_bRender)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLandCursor::Render()
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


	return S_OK;
}


HRESULT CLandCursor::Ready_Components()
{
	/* For.Com_Texture */

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.iStartTex = 0;
	TextureDesc.iEndTex = 7;
	TextureDesc.fSpeed = 0.1f;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LandCursor"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), &TextureDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 2.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLandCursor::SetUp_RenderState()
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

HRESULT CLandCursor::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);



	return S_OK;
}


CLandCursor * CLandCursor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLandCursor*		pInstance = new CLandCursor(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLandCursor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CLandCursor::Clone(void* pArg)
{
	CLandCursor*		pInstance = new CLandCursor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLandCursor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLandCursor::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);

}
