#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "RenderingObject.h"
#include "FSM.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

BEGIN(Client)
class CPlayer final : public CLandObject
{
public:
	enum CameraMode { CAMERA_BACKHEAD, CAMERA_FPS, CAMERA_ZOOM, CAMERA_EXPENDING, CAMERA_REDUCING, CAMERA_MOVING, CAMERA_END };
	enum PARTS { PART_HAND, PART_HEAD, PART_BODY, PART_WEAPON, PART_EQUIPPED, PART_WEAPONLIGHT, PART_END };
	enum UI { UI_AIM, UI_ZOOMAIM, UI_ZOOMAIMSHAPE, UI_END };
	enum WEAPONS { RAMBO, SPARROW, FLAMESHAKER, Grenade, WEAPONS_END };

private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Strike_Force(_float3 _Vector3D) { m_vInertia += _Vector3D; }
	void Add_RenderObject_Parts();

public:
	HRESULT SetUP_CameraTransform(LEVELID eLevelId);
	_int Get_HP() { return m_iHP; }
	_int Get_MaxHP() { return m_iMaxHP; }
	_float Get_MP() { return m_fMP; }
	_float Get_Charge() { return m_fCharge; }
	_float Get_Fever() { return m_fFever; }
	_float Get_Immortal() { return m_fHat; }
	_bool Get_bFever() { return m_bFever; }
	WEAPONS Get_Weapon() { return m_iCurWeapon; }
	WAY Get_Way();
	_bool Get_Damaged() { return m_fDamaged > 0; }
	_float Get_SkillCooltime(_int _iSkillnum) { return m_fSkillCooltime[_iSkillnum]; }
	_int Get_CurBullet() { return m_iBullet[(int)m_iCurWeapon]; }

	void Set_FPSOFF(_bool _bMode) { m_bFPSOFF = _bMode; }
	void PlusHP(_int _iAtt);
	void HitPlayer(_int _iAtt);
	void Reset_Inertia();
	void Plus_Fever(_float _value) { m_fFever += _value; }

private:
	/*Component*/
	CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pTransformCamera = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };
	CRenderingObject* m_pParts[PART_END] = { nullptr };
	CRenderingObject* m_pUI[UI_END] = { nullptr };
	CGameObject* m_pShield = { nullptr };
	CGameObject* m_pGauge_Grenade = { nullptr };
	CGameObject* m_pGrenadeRange = { nullptr };
	CFSM* m_pFSM = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	HRESULT SetUp_FSM();
	void Mouse_Moving(_float fTimeDelta);
	void Killswitch_Shield();
	void Apply_Recoil(_float Recoil);

	struct CameraState
	{
		float pitch;     // 카메라의 피치 (위아래 회전)
		float yaw;       // 카메라의 요 (좌우 회전)
		bool isRecoiling;
		float recoilTime;
		float maxRecoilTime;
		float recoilPitchIncrease; // 초당 피치 증가량
		float maxPitch;
	};

	CameraState	m_Camera;


private:
	_float3					m_vInertia = { 0.f, 0.f, 0.f }; //관성
	_float3					m_vTheta = { 0.f, 0.f, 0.f }; //플레이어가 실제로 바라보는 각도
	_float					m_fFrictionRate = { 0.9f }; // 마찰
	_int					m_iActCooltime = { 0 }; // 후딜레이
	_float					m_fSkillCooltime[3] = { 1.f, 2.f, 5.f }; //개별 스킬 쿨타임 -> 0. 구르기, 1. 수류탄, 2. 쉴드
	_float4x4				m_CollisionMatrix;
	_float					m_fAimDistance;
	CameraMode				m_eCameraMode = { CAMERA_BACKHEAD };
	POINT					m_ptOldMousePos = {};
	_int					m_iBullet[3] = {};	//잔탄 람보-스페로우-플레임셰이커
	_int					m_iMAXBullet[3]{ 30, 7, 15 };
	WEAPONS					m_iCurWeapon = { RAMBO };
	_float					m_fWeaponCooltime[WEAPONS_END] = { 0.15f, 1.f, 0.05f, 0.f };	//무기별 연사 쿨타임 설정값
	_float					m_fRemainWeaponCooltime[WEAPONS_END] = { 0.f };	//남은 무기 쿨타임
	_int					m_iHP = { 3 };
	_int					m_iMaxHP = { 3 };
	_float					m_fCharge = { 0.1f }; //차지 누른 시간
	_bool					m_bFPSOFF = { false }; //FPS 모드 잠금
	_float					m_fHat = { 0.f }; //무적시간
	_float					m_fMP = { 6.f }; //마나
	_float					m_fFever = { 0.f }; //피버게이지 [0.f ~ 6.f]
	_bool					m_bFever = { false };
	_float					m_fAfterImageTime = 0.f;
	_float					m_fDamaged = { 0.f };

private:
	void Key_Input(_float fTimeDelta);
	void Friction(_float _FrictionRate);
	void CameraAround();
	void PlayerStartPosing();
	HRESULT Clone_PlayerParts();
	HRESULT Clone_UI();
	void ShootingCurWeapon();
	void FrameMana(_float _fTimeDelta);
	void FrameFever(_float _fTimeDelta);
	void FrameSkillCooltime(_float _fTimeDelta);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END