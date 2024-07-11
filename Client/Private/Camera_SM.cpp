#include "stdafx.h"
#include "Camera_SM.h"

#include "GameInstance.h"
#include "Camera_Manager.h"

CCamera_SM::CCamera_SM(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CCamera_SM::CCamera_SM(const CCamera_SM& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCamera_SM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_SM::Initialize(void* pArg)
{
	/* 설정한 상대적 위치 초기값 저장 */
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
	m_vEye = pDesc->vEye;
	m_vAt = pDesc->vAt;
	m_fFovy = pDesc->fFovy;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;
	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;
	m_eCurLevel = pDesc->eLevel;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(m_eCurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));

	return S_OK;
}

void CCamera_SM::Priority_Update(_float fTimeDelta)
{
	if (m_bEndingCamera == 0)
	{

		/* 플레이어 위치 추출*/
		_float3 pPlayerPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);

		m_pTransformCom->GOTO(pPlayerPos + _float3{ 0.f, 5.f, -4.f });
		m_pTransformCom->LookAt(pPlayerPos + _float3{ 0.f, 1.f, 0.01f });
	}
	else if (m_bEndingCamera == 1)
	{
		//엔딩카메라
	}
	else if (m_bEndingCamera == 2)
	{
		//m_pTransformCom->LookAt(m_pTarget->Get_Pos());
		m_pTransformCom->LookAt( m_pTargetTransform->Get_State(CTransform::STATE_POSITION));
	}
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_pTransformCom->Get_WorldMatrix_Inverse());
	_float4x4		ProjMatrix{};
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_fFovy, (_float)g_iWinSizeX / g_iWinSizeY, m_fNear, m_fFar));
}

int CCamera_SM::Update(_float fTimeDelta) 
{
	if (m_bEndingCamera)
	{
		m_pTransformCom->Move(_float3{ 0.f, 0.f, 10.f * fTimeDelta });
	}

	return OBJ_NOEVENT; 
}

void CCamera_SM::Late_Update(_float fTimeDelta) 
{
	if (Get_Pos().z >= 84)
	{
		if (m_bEndingCamera == 0)
		{
			m_pTransformCom->GOTO(_float3{ 75.f, 5.f, 18.f });
			m_pGameInstance->StopSound(0);
			m_pGameInstance->PlayBGM(TEXT("induction.wav"), 1.f, true);
			m_bEndingCamera = 1;
		}
		else
		{
			m_pTransformCom->GOTO(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + m_pTargetTransform->Get_State(CTransform::STATE_LOOK));
			m_bEndingCamera = 2;
		}
	}


}

HRESULT CCamera_SM::Render() { return S_OK; }

void CCamera_SM::Set_CameraView(_float3 _vEye, _float3 _vAt, _float _fFovy)
{
	m_vEye = _vEye;
	m_vAt = _vAt;
	m_fFovy = _fFovy;
}

void CCamera_SM::Set_CameraAngle(_float _fRadian)
{
	m_fFovy = _fRadian;
}

HRESULT CCamera_SM::Set_CameraMode(_int iMode)
{

	return S_OK;
}

HRESULT CCamera_SM::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CCamera_SM* CCamera_SM::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_SM* pInstance = new CCamera_SM(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_SM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_SM::Clone(void* pArg)
{
	CCamera_SM* pInstance = new CCamera_SM(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_SM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_SM::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

}
