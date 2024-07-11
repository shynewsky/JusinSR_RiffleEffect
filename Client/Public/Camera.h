#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GameInstance.h"

BEGIN(Engine)
class CTransform;
class CTexture;
END

BEGIN(Client)
class CRenderingObject;
class CCamera final : public CGameObject
{
public:
	enum CAMERAMODE { CAMERA_BACKHEAD, CAMERA_FPS, CAMERA_ZOOM, CAMERA_EXPENDING, CAMERA_REDUCING, CAMERA_MOVING, CAMERA_CENEMA, CAMERA_END };
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
	CCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

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
	void	Moving_Camera(_float3* _pDestination, _float _fFovy, CAMERAMODE	_m_eDestination);
	void	Set_Recoil(_float recoil) { m_fRecoil = recoil; };

public:
	CAMERAMODE Get_CurrentCameaMode() { return m_eCamraMode; }
	void Set_BosPos(_float3& vPos) { m_vBossPos = vPos; }
	void Set_MyPos(_float3& vPos) { m_fMyPos = vPos; }

private:
	void Cenema_Camera(_float fTimeDelta);
	
private:
	CTransform*			m_pTransformCom = { nullptr }; // 카메라의 월드행렬, 플레이어 위치와 마우스커서가 반영된 카메라 위치/룩벡터
	CTransform*			m_pTargetTransform = { nullptr }; // 카메라 타겟의 트랜스폼
	CGameObject*		m_pTarget = { nullptr };
	/* 플레이어 기준 상대적 위치 -> 객체생성할때 객체마다 다르게 부여 */
	_float3				m_vEye = {};		// 상대적 위치벡터 -> 플레이어 위치에 고정적으로 움직임
	_float3				m_vAt = {};			// 상대적 룩벡터 -> 마우스 커서에 의해 유동적으로 변경됨
	_float				m_fFovy = {};		// 시야 초기화값 고정
	_float				m_fNear = {};		// 시야 초기화값 고정
	_float				m_fFar = {};		// 시야 초기화값 고정
	_float				m_fSpeedPerSec{};		// 이동속도 고정
	_float				m_fRotationPerSec{};	// 회전속도 고정


	_float3*			m_pDestnation = {};	//카메라 이동 목적지
	bool				m_bMovingCamera = { false }; //카메라 추적 여부
	CAMERAMODE			m_eDestination = { CAMERA_END }; // 이동 후 안착할 모드

	POINT				m_ptOldMousePos = {}; // 기준이 되는 마우스 커서
	CAMERAMODE			m_eCamraMode = { CAMERA_BACKHEAD };
	bool				m_CameraHOLD = {false}; //좌우 이동 잠금
	int					m_iMovingTick = { 0 }; //카메라 시점변경 틱
	LEVELID				m_eCurLevel = { LEVEL_END };

	//시네마 카메라
	_float3				m_vBossPos = {};
	_float3				m_fMyPos = _float3(50.f, 0.5f, 17.f);
	_float				m_fCenemaTime = 0.f;
	//반도
	_float				m_fRecoil = 0.f;
	_float				m_fPrevRecoil = 0.f;
	_float				m_fBackRecoilTime = 1.f;
	_float				m_fBackRecoilSumTime = 0.f;
	_int				m_fBackRecoilCount = 0;

private:
	HRESULT Ready_Components();

public:
	static CCamera* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END