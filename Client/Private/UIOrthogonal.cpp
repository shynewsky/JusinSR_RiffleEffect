#include "stdafx.h"
#include "..\Public\UIOrthogonal.h"

#include "GameInstance.h"
#include "Bullet.h"

CUIOrthogonal::CUIOrthogonal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CUIOrthogonal::CUIOrthogonal(const CUIOrthogonal & Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CUIOrthogonal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIOrthogonal::Initialize(void * pArg)
{
	if (pArg)
	{
		Orthogonal_DESC* pDesc = static_cast<Orthogonal_DESC*>(pArg);

		m_vWorldPos = pDesc->vPos;
		m_fSizeX = pDesc->fSizeX;
		m_fSizeY = pDesc->fSizeY;
		m_bCull = pDesc->bCull;
	}



	return S_OK;
}

void CUIOrthogonal::Priority_Update(_float fTimeDelta)
{
}

int CUIOrthogonal::Update(_float fTimeDelta)
{
	if (this->m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CUIOrthogonal::Late_Update(_float fTimeDelta)
{
	if(m_bRenderState)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
	}
	else
	{

	}
}

HRESULT CUIOrthogonal::Render()
{	



	return S_OK;
}


HRESULT CUIOrthogonal::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_prevProject);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	return S_OK;
	
}

HRESULT CUIOrthogonal::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_prevProject);


	return S_OK;
}

void CUIOrthogonal::Set_OrthoLH(CTransform* pTransform)
{
	_float4x4 ViewMatrix, ProjMatrix;
	_float3 ScreenPosition;
	D3DVIEWPORT9 viewport;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);


	_float3 pos = m_vWorldPos;
	D3DXVec3TransformCoord(&pos, &pos, &ViewMatrix);
	m_fZ = pos.z;

	//m_bCull이 True면 카메라 뒷면 일때 렌더링 안한다 false 이면 내가 알아서 true false 한다.
	if(m_bCull)
	{
		if (pos.z < 0)
		{
			m_bRenderState = false;
			return;
		}
		else
			m_bRenderState = true;
	}

	D3DXVec3TransformCoord(&pos, &pos, &ProjMatrix);

	_float3 screenPosition;
	screenPosition.x = ((pos.x + 1.0f) * 0.5f) * g_iWinSizeX;
	screenPosition.y = ((1.0f - pos.y) * 0.5f) * g_iWinSizeY;


	m_fX = screenPosition.x;
	m_fY = screenPosition.y;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);


	pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));
	pTransform->Scaling(m_fSizeX, m_fSizeY, 1.f);
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);
}


void CUIOrthogonal::Free()
{
	__super::Free();

}
