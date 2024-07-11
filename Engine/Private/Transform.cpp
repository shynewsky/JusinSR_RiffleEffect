#include "..\Public\Transform.h"

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device }
{
}

CTransform::CTransform(const CTransform & Prototype)
	: CComponent{ Prototype }
	, m_WorldMatrix{ Prototype.m_WorldMatrix } 
{
}

//STATE_UP 사용 금지
void CTransform::Safe_Setting_State(STATE _eState, _float3& _vState)
{
	_float3 vCross = {};
	D3DXVec3Normalize(&_vState, &_vState);
	switch (_eState)
	{
	case Engine::CTransform::STATE_RIGHT:
		memcpy(&m_WorldMatrix.m[STATE_RIGHT][0], &_vState, sizeof(_float3));
		memcpy(&m_WorldMatrix.m[STATE_LOOK][0], D3DXVec3Cross(&vCross, &_vState, &Get_State(STATE_UP)), sizeof(_float3));
		break;
	case Engine::CTransform::STATE_UP:
		//진짜쓰면안됨
		break;
	case Engine::CTransform::STATE_LOOK:
		memcpy(&m_WorldMatrix.m[STATE_LOOK][0], &_vState, sizeof(_float3));
		memcpy(&m_WorldMatrix.m[STATE_RIGHT][0], D3DXVec3Cross(&vCross, &Get_State(STATE_UP), &_vState), sizeof(_float3));
		break;
	case Engine::CTransform::STATE_POSITION:
		memcpy(&m_WorldMatrix.m[_eState][0], &_vState, sizeof(_float3));
		break;
	default:
		break;
	}
}

void CTransform::Set_Billboard()
{
	_float4x4		ViewMatrix{};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3		vScaled = Get_Scaled();

	for (size_t i = 0; i < CTransform::STATE_POSITION; i++)
	{
		if (i == CTransform::STATE_UP)
			continue;
		Set_State(CTransform::STATE(i), *(_float3*)&ViewMatrix.m[i] * *((_float*)&vScaled + i));
	}
}

HRESULT CTransform::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (pArg == nullptr)
		return S_OK;

	TRANSFORM_DESC*		pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_Transform()
{
	return m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
}

//곱하기연산임
void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	_float3		vRight = Get_State(STATE_RIGHT);
	Set_State(STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * fScaleX);

	_float3		vUp = Get_State(STATE_UP);
	Set_State(STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * fScaleY);

	_float3		vLook = Get_State(STATE_LOOK);
	Set_State(STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * fScaleZ);

}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = Get_State(STATE_LOOK);

	vPosition += *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = Get_State(STATE_LOOK);

	vPosition -= *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vRight = Get_State(STATE_RIGHT);

	vPosition += *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vRight = Get_State(STATE_RIGHT);

	vPosition -= *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vUp = Get_State(STATE_UP);

	vPosition += *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::GOTO(_float3 _vPos)
{
	Set_State(STATE_POSITION, _vPos);
}

void CTransform::Turn(const _float3& vAxis, _float fTimeDelta)
{
	_float3		vRight = Get_State(STATE_RIGHT);
	_float3		vUp = Get_State(STATE_UP);
	_float3		vLook = Get_State(STATE_LOOK);
		
	_float4x4		RotationMatrix{};

	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, m_fRotationPerSec * fTimeDelta);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}
void CTransform::TurnBackwards(_float fTimeDelta)
{
	_float3 vAxis = _float3(1.0f, 0.0f, 0.0f);

	_float3 vRight = Get_State(STATE_RIGHT);
	_float3 vUp = Get_State(STATE_UP);
	_float3 vLook = Get_State(STATE_LOOK);
	_float3 vPosition = Get_State(STATE_POSITION);

	_float3 vBottomCenter = vPosition - vUp * 0.5f; 

	_float4x4 TranslationMatrixToOrigin{};
	D3DXMatrixTranslation(&TranslationMatrixToOrigin, -vBottomCenter.x, -vBottomCenter.y, -vBottomCenter.z);

	_float4x4 TranslationMatrixBack{};
	D3DXMatrixTranslation(&TranslationMatrixBack, vBottomCenter.x, vBottomCenter.y, vBottomCenter.z);

	_float4x4 RotationMatrix{};
	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, m_fRotationPerSec * fTimeDelta);

	_float4x4 TransformMatrix = TranslationMatrixToOrigin * RotationMatrix * TranslationMatrixBack;

	D3DXVec3TransformCoord(&vPosition, &vPosition, &TransformMatrix);
	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);  
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
	Set_State(STATE_POSITION, vPosition);
	
}

void CTransform::Rotation(const _float3& vAxis, _float fRadian)
{
	m_fRotation = D3DXToDegree(fRadian);

	_float3		vScaled = Get_Scaled();

	_float3		vRight = _float3(vScaled.x, 0.f, 0.f);
	_float3		vUp = _float3(0.f, vScaled.y, 0.f);
	_float3		vLook = _float3(0.f, 0.f, vScaled.z);

	_float4x4		RotationMatrix{};

	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, fRadian);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::LookAt(const _float3& vAt)
{
	_float3		vScaled = Get_Scaled();
	
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = vAt - vPosition;
	_float3		vRight{}, vUp{};

	D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Cross(&vUp, &vLook, &vRight);

	Set_State(STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * vScaled.x);
	Set_State(STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * vScaled.y);
	Set_State(STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * vScaled.z);
}

void CTransform::LookAt_ForLandObject(const _float3& vAt)
{
	_float3		vScaled = Get_Scaled();

	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vUp = Get_State(STATE_UP);
	_float3		vLook = vAt - vPosition;
	_float3		vRight{};

	D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Cross(&vLook, &vRight, &vUp);

	Set_State(STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * vScaled.x);
	Set_State(STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * vScaled.z);
}

void CTransform::Go_PosTarget(_float fTimeDelta, _float3 TargetPos)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	_float3 vDir = TargetPos - vPos;

	D3DXVec3Normalize(&vDir, &vDir);

	vPos += vDir * fTimeDelta * m_fSpeedPerSec;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_PosDir(_float fTimeDelta, _float3 vecDir)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);

	_float3 vDir = vecDir;

	D3DXVec3Normalize(&vDir, &vDir);

	vPos += vDir * fTimeDelta * m_fSpeedPerSec;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Move(_float3& vPlusPos)
{
	m_WorldMatrix._41 += vPlusPos.x;
	m_WorldMatrix._42 += vPlusPos.y;
	m_WorldMatrix._43 += vPlusPos.z;
}

CTransform * CTransform::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTransform*		pInstance = new CTransform(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CTransform::Clone(void* pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
