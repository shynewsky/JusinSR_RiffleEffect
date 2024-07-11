#include "stdafx.h"
#include "..\Public\Camera.h"

#include "GameInstance.h"
#include "Camera_Manager.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CCamera::CCamera(const CCamera & Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
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

void CCamera::Priority_Update(_float fTimeDelta)
{
	POINT		ptMouse = {};
	GetCursorPos(&ptMouse);
	_long	MouseMove = {};
	_float  fXRecoil = 0.f;
	//x반동 구현부
	if (m_fRecoil < 0.f)
	{
		if (m_fRecoil > -20.f)
		{
			fXRecoil = m_fRecoil * 0.3f;
			rand() % 2 == 1 ? fXRecoil *= -1.f : fXRecoil;
			m_fPrevRecoil = -25.f;
		}
		else
		{
			m_fPrevRecoil = m_fRecoil;
		}
		
		m_fPrevRecoil *= -1.f;
		m_fBackRecoilSumTime = 0.f;
	}
	else if(m_fRecoil > 0.f)
	{
		m_fBackRecoilSumTime += m_fRecoil;
		if (m_fBackRecoilSumTime >= m_fPrevRecoil)
		{
			m_fBackRecoilSumTime = 0.f;
			m_fRecoil = 0.f;
		}
			
	}

	MouseMove = ptMouse.x - g_iWinSizeX * 0.5;
	if (m_eCamraMode != CAMERA_BACKHEAD)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta * (MouseMove + fXRecoil) * 0.05f);
	}

	MouseMove = ptMouse.y - g_iWinSizeY * 0.5;
	if (m_eCamraMode != CAMERA_BACKHEAD)
	{
		_float3 fAxisRight = _float3{ m_pTransformCom->Get_State(CTransform::STATE_RIGHT).x, 0.f, m_pTransformCom->Get_State(CTransform::STATE_RIGHT).z };
		D3DXVec3Normalize(&fAxisRight, &fAxisRight);
		m_pTransformCom->Turn(fAxisRight, fTimeDelta * (MouseMove + m_fRecoil) * 0.05f);
	}

	if (m_fRecoil < 0.f)
	{
		if (m_fRecoil > -20.f)
			m_fRecoil *= -0.1f;
		else
			m_fRecoil *= -0.02f;

	}

	m_ptOldMousePos = ptMouse;

	/* 플레이어 위치 추출*/
	_float3 pPlayerPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_float3 pPlayerLook = m_pTargetTransform->Get_State(CTransform::STATE_LOOK);
	


	switch (m_eCamraMode)
	{
	case CAMERA_BACKHEAD:
		m_pTransformCom->GOTO(pPlayerPos - pPlayerLook * 5.f + _float3{ 0.f, 3.5f, 0.f });
		m_pTransformCom->LookAt(pPlayerPos + pPlayerLook + _float3{ 0.f, 1.f, 0.f });
		break;
	case CAMERA_FPS:
		m_pTransformCom->GOTO(pPlayerPos - pPlayerLook * 1.f + _float3{ 0.f, 0.6f, 0.f });
		break;
	case CAMERA_ZOOM:
		m_pTransformCom->GOTO(pPlayerPos);
		m_pTransformCom->LookAt(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		break;
	case CAMERA_EXPENDING:
		m_pTransformCom->GOTO(pPlayerPos - pPlayerLook * (5.f - 4.f * (m_iMovingTick) / 30.f) + _float3{ 0.f, 3.5f - 2.5f * (m_iMovingTick) / 30.f, 0.f });
		m_pTransformCom->LookAt(pPlayerPos + pPlayerLook * 2.f * m_iMovingTick / 30.f + _float3{ 0.f, 1.f ,0.f } *(40 - m_iMovingTick) / 30.f);
		++m_iMovingTick;
		if (m_iMovingTick >= 30)
		{
			m_iMovingTick = 0;
			Set_CameraMode(int(CAMERA_FPS));
		}
		break;
	case CAMERA_REDUCING:
		m_pTransformCom->GOTO(pPlayerPos - pPlayerLook * (5.f * m_iMovingTick / 30.f) + _float3{ 0, 3.5f * m_iMovingTick / 30.f, 0.f });
		m_pTransformCom->LookAt(pPlayerPos + pPlayerLook + _float3{ 0.f, 1.f ,0.f } *(m_iMovingTick) / 30.f);
		++m_iMovingTick;
		m_fFovy = D3DXToRadian(30 + m_iMovingTick);
		if (m_iMovingTick >= 30)
		{
			Set_CameraMode(int(CAMERA_BACKHEAD));
			m_iMovingTick = 0;
		}
		break;
	case CAMERA_MOVING:
		m_pTransformCom->Move(*m_pDestnation);
		m_pTransformCom->LookAt(pPlayerPos + pPlayerLook + _float3{ 0.f, 1.f ,0.f });
		++m_iMovingTick;
		if (m_iMovingTick > 30)
		{
			Set_CameraMode((int)m_eDestination);
			m_iMovingTick = 0;
		}
		break;

	case CAMERA_CENEMA:
		Cenema_Camera(fTimeDelta);
		break;
	default:
		break;
	}
	
	/* 장치에 뷰랑 투영행렬을 한번 셋팅 해주면 다시 갱신해 주기 전까지 모든 객체들을 렌더링시에 셋팅해준 행렬을 곱하면서 그리게 된다. */
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_pTransformCom->Get_WorldMatrix_Inverse());
	_float4x4		ProjMatrix{};
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_fFovy, (_float)g_iWinSizeX / g_iWinSizeY, m_fNear, m_fFar));

}

int CCamera::Update(_float fTimeDelta) { return OBJ_NOEVENT; }

void CCamera::Late_Update(_float fTimeDelta) {}

HRESULT CCamera::Render() { return S_OK; }

void CCamera::Set_CameraView(_float3 _vEye, _float3 _vAt, _float _fFovy)
{
	m_vEye = _vEye;
	m_vAt = _vAt;
	m_fFovy = _fFovy;
}

void CCamera::Set_CameraAngle(_float _fRadian)
{
	m_fFovy = _fRadian;
}

HRESULT CCamera::Set_CameraMode(_int iMode)
{

	switch (iMode)
	{
	case 0:
		m_CameraHOLD = true;
		m_fFovy = D3DXToRadian(60.f);
		break;
	case 1:
		{
		m_CameraHOLD = false;
		m_fFovy = D3DXToRadian(60.f);
		_float3 vPos = m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_LandCursor"))->Get_Pos();
		vPos.y += 0.75f;
		m_pTransformCom->LookAt(vPos);
		}
		break;
	case 2:
		m_CameraHOLD = false;
		m_fFovy = D3DXToRadian(30.f);
		break;
	case 3: //expending
		break;
	case 4: //reducing
		break;

	case 6: //시네마
		m_fFovy = D3DXToRadian(60.f);
		break;
	default:
		return E_FAIL;
	}
	m_eCamraMode = (CAMERAMODE)iMode;

	return S_OK;
}

void CCamera::Moving_Camera(_float3* _pDestination, _float _fFovy, CAMERAMODE _m_eDestination)
{
	m_bMovingCamera = true;
	m_pDestnation = _pDestination;
	m_fFovy = _fFovy;
	m_eDestination = _m_eDestination;
	m_eCamraMode = CAMERA_MOVING;
}

void CCamera::Cenema_Camera(_float fTimeDelta)
{
	m_fCenemaTime += fTimeDelta;
	if (m_fCenemaTime >= 6.f)
	{
		m_fCenemaTime = 0.f;
		Set_CameraMode(0);
		m_pGameInstance->Set_RenderUI(true);
	}
	if(m_fFovy>= D3DXToRadian(30.f))
		m_fFovy -= 0.002f;

	m_pTransformCom->GOTO(m_fMyPos);
	if (m_fMyPos.z == 60.f)
	{
		m_vBossPos = m_pGameInstance->Find_Object(m_eCurLevel, TEXT("Layer_Monster"))->Get_Pos();
	}
	m_pTransformCom->LookAt(m_vBossPos);
}

HRESULT CCamera::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	//TransformDesc.fSpeedPerSec = m_fSpeedPerSec;
	//TransformDesc.fRotationPerSec = m_fRotationPerSec;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CCamera * CCamera::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera*		pInstance = new CCamera(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CCamera::Clone(void* pArg)
{
	CCamera*		pInstance = new CCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

}
