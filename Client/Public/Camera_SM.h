#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "GameInstance.h"

BEGIN(Engine)
class CTransform;
class CTexture;
END

BEGIN(Client)
class CRenderingObject;

class CCamera_SM :
	public CGameObject
{
public:
	/* 플레이어 기준 상대적 위치 -> 객체생성할때 객체마다 다르게 부여 */
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_float3			vEye = { 10.f, 0.f, -10.f };	 // 상대적 위치벡터 -> 플레이어 위치에 고정적으로 움직임
		_float3			vAt = { 0.f, 0.f, 0.f };	 // 상대적 룩벡터 -> 마우스 커서에 의해 유동적으로 변경됨
		_float			fFovy = { 90.f };	 // 시야 초기화값 고정
		_float			fNear = { 0.1f };	 // 시야 초기화값 고정
		_float			fFar = { 1000.f };	 // 시야 초기화값 고정
		LEVELID			eLevel; //적용할 레벨
	}CAMERA_DESC;

private:
	CCamera_SM(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera_SM(const CCamera_SM& Prototype);
	virtual ~CCamera_SM() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_CameraView(_float3 _vEye, _float3 _vAt, _float _fFovy);
	void Set_CameraAngle(_float _fRadian);
	HRESULT Set_CameraMode(_int iMode);
	HRESULT Set_TargetTransform(CTransform* _pTransform) { m_pTargetTransform = _pTransform; }
	HRESULT Set_Target(CGameObject* _pTarget) { m_pTarget = _pTarget; }

private:
	CTransform* m_pTransformCom = { nullptr }; // 카메라의 월드행렬, 플레이어 위치와 마우스커서가 반영된 카메라 위치/룩벡터
	CTransform* m_pTargetTransform = { nullptr }; // 카메라 타겟의 트랜스폼
	CGameObject* m_pTarget = { nullptr };
	/* 플레이어 기준 상대적 위치 -> 객체생성할때 객체마다 다르게 부여 */
	_float3				m_vEye = {};		// 상대적 위치벡터 -> 플레이어 위치에 고정적으로 움직임
	_float3				m_vAt = {};			// 상대적 룩벡터 -> 마우스 커서에 의해 유동적으로 변경됨
	_float				m_fFovy = {};		// 시야 초기화값 고정
	_float				m_fNear = {};		// 시야 초기화값 고정
	_float				m_fFar = {};		// 시야 초기화값 고정
	_float				m_fSpeedPerSec{};		// 이동속도 고정
	_float				m_fRotationPerSec{};	// 회전속도 고정

	LEVELID				m_eCurLevel = { LEVEL_END };
	_int				m_bEndingCamera = { 0 };
private:
	HRESULT Ready_Components();

public:
	static CCamera_SM* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END