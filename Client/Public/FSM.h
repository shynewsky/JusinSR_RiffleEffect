#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CFSM
{
public:
	typedef struct
	{
		int iAniNum = 0; //세트당 몇장인지
		int iWayNum = 0; //몇방향 있는지
	}Frame_DESC;
	enum OBJID { OBJID_PLAYER, OBJID_MONSTER, OBJID_PLAYERBULLET, OBJID_ENEMYBULLET, OBJID_ENEMYATTACK, OBJID_MOUSE, OBJID_END };
	enum OBJSTATE { OBJSTATE_IDLE, OBJSTATE_ROLL, OBJSTATE_MOVE, OBJSTATE_BACKMOVE, OBJSTATE_JUMP, OBJSTATE_DIE, OBJSTATE_ATTACK
		, OBJSTATE_TPSCHANGE, OBJSTATE_TPSIDLE, OBJSTATE_RENDEROFF, OBJSTATE_WEAPONEQUIPED, OBJSTATE_WEAPON, OBJSTATE_WEAPONREINFORCE
		, OBJSTATE_APPEAR, OBJSTATE_FLY, OBJSTATE_FLYDOWN, OBJSTATE_FLYUP, OBJSTATE_GROGGY, OBJSTATE_PATTERNSTART, OBJSTATE_THROW, OBJSTATE_DEAD // 보스 Nero 상태	
		, OBJSTATE_END};

private:
	CFSM();
	~CFSM();

public:
	HRESULT Initialize(void* pArg);
	HRESULT Update();
	_int Late_Update(_float fTimeDelta);
	void Release();

public:
	CTexture* Get_CurTextureCom() {	return m_pTextureCom[m_eCurstate]; }
	OBJSTATE Get_CurState() {
		return m_eCurstate; }
	WAY Get_Way() { return m_eWay; }
	HRESULT Set_TextureCom(OBJSTATE _eSTate, CTexture* _pTexture);
	void Set_Anitype(ANITYPE _eAnitype) { m_eAnitype = _eAnitype; }
	void Set_State(OBJSTATE _eState) { m_eCurstate = _eState; }
	void Set_Way(WAY _eWay);
	void Set_FrameSpeed(OBJSTATE _eState, _float _fSpeed);

public:
	HRESULT Add_FrameDesc(OBJSTATE _eState, Frame_DESC _Desc);
	Frame_DESC find_FrameDesc(OBJSTATE _eState);
	void Turn_Frame(WAY _iWay);
	_int Move_Frame(const _float fTimer);
	void Reset_Frame();

private:
	CTexture* m_pTextureCom[OBJSTATE_END];

private:
	map<OBJSTATE, Frame_DESC> m_mapFrame;
	OBJSTATE m_eCurstate = { OBJSTATE_END };
	OBJSTATE m_ePrestate = { OBJSTATE_END };
	ANITYPE	 m_eAnitype = { ANI_LOOP };
	WAY		 m_eWay = { WAY_FRONT };

private:
	bool ConversionCheck();
	Frame_DESC Get_CurFrameDesc();

public:
	static CFSM* Create(void* pArg);
	static void Destroy(CFSM* pInstance);
};

END