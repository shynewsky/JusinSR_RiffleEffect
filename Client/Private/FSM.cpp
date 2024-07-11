#include "stdafx.h"
#include "FSM.h"
#include "Client_Defines.h"
#include "Texture.h"

CFSM::CFSM()
{
}

CFSM::~CFSM()
{
}

HRESULT CFSM::Initialize(void* pArg)
{
	ZeroMemory(m_pTextureCom, OBJSTATE_END);
	return S_OK;
}

HRESULT CFSM::Update()
{

	return S_OK;
}

_int CFSM::Late_Update(_float fTimeDelta) // 사망프레임이 끝나면 OBJ_DEAD를 반환한다.
{
	if (!ConversionCheck()) //상태가 바뀌면
	{
		m_pTextureCom[m_eCurstate]->Set_Frame(Get_CurFrameDesc().iAniNum * m_eWay, Get_CurFrameDesc().iAniNum * (m_eWay + 1), 0.1f);
	}
	
	if (OBJ_DEAD == Move_Frame(fTimeDelta))
		return OBJ_DEAD;
	return OBJ_NOEVENT;
}

void CFSM::Release() {}

bool CFSM::ConversionCheck()
{
	if (m_ePrestate != m_eCurstate)
	{
		switch (m_eCurstate)
		{
		case OBJSTATE_IDLE:
			m_eAnitype = ANI_LOOP;
			break;
		case OBJSTATE_ROLL:
			m_eAnitype = ANI_BACKTOIDLE;
			break;
		case OBJSTATE_ATTACK:
			m_eAnitype = ANI_BACKTOIDLE;
		case OBJSTATE_MOVE:
			m_eAnitype = ANI_LOOP;
			break;
		case OBJSTATE_DIE:
			m_eAnitype = ANI_STOP;
			m_eWay = WAY_FRONT;
			break;
		case OBJSTATE_TPSIDLE:
			m_eAnitype = ANI_STOP;
			break;
		case OBJSTATE_TPSCHANGE:
			m_eAnitype = ANI_BACKTOTPSIDLE;
			break;
		case OBJSTATE_WEAPON:
			m_eAnitype = ANI_LOOP;
			break;
		case OBJSTATE_WEAPONREINFORCE:
			m_eAnitype = ANI_LOOP;
			break;
		case OBJSTATE_APPEAR:
			m_eAnitype = ANI_STOP;
			break;
		case OBJSTATE_FLY:
			m_eAnitype = ANI_LOOP;
			break;
		case OBJSTATE_FLYDOWN:
			m_eAnitype = ANI_STOP;
			break;
		case OBJSTATE_FLYUP:
			m_eAnitype = ANI_STOP;
			break;
		case OBJSTATE_GROGGY:
			m_eAnitype = ANI_LOOP;
			break;
		case OBJSTATE_PATTERNSTART:
			m_eAnitype = ANI_STOP;
			break;
		case OBJSTATE_THROW:
			m_eAnitype = ANI_STOP;
		case OBJSTATE_DEAD:
			m_eAnitype = ANI_STOP;
			break;
		default:
			break;
		}
		Reset_Frame();
		m_ePrestate = m_eCurstate;
		return false;
	}
	return true;
}

void CFSM::Set_Way(WAY _eWay)
{
	Turn_Frame(_eWay);
	m_eWay = _eWay;
}

void CFSM::Set_FrameSpeed(OBJSTATE _eState, _float _fSpeed)
{
	m_pTextureCom[_eState]->Set_FrameSpeed(_fSpeed);
}

HRESULT CFSM::Add_FrameDesc(OBJSTATE _eState, Frame_DESC _Desc)
{
	auto iter = m_mapFrame.find(_eState);

	if (iter != m_mapFrame.end())
	{
		return E_FAIL;
	}

	m_mapFrame.emplace(_eState, _Desc);

	return S_OK;
}

CFSM::Frame_DESC CFSM::find_FrameDesc(OBJSTATE _eState)
{
	auto iter = m_mapFrame.find(_eState);

	if (m_mapFrame.end() == iter)
	{
		return Frame_DESC();
	}

	return (*iter).second;
}

CFSM::Frame_DESC CFSM::Get_CurFrameDesc()
{
	return find_FrameDesc(m_eCurstate);
}

HRESULT CFSM::Set_TextureCom(OBJSTATE _eSTate, CTexture* _pTexture)
{
	if (nullptr == _pTexture)
		return E_FAIL;

	m_pTextureCom[_eSTate] = _pTexture;

	return S_OK;
}

void CFSM::Turn_Frame(WAY _iWay)
{
	m_eWay = _iWay;
	Frame_DESC _Desc = Get_CurFrameDesc();
	_Desc.iAniNum;
	m_pTextureCom[m_eCurstate]->Turn_Frame(_iWay * _Desc.iAniNum, _Desc.iAniNum * (_iWay + 1));
}

_int CFSM::Move_Frame(const _float fTimer)
{
	bool bLoop(true);
	if (m_eCurstate == OBJSTATE_ROLL || m_eAnitype == ANI_STOP || m_eAnitype == ANI_BACKTOTPSIDLE)
		bLoop = false;
	if (m_pTextureCom[m_eCurstate]->Move_Frame(fTimer, bLoop))
	{
		if (OBJSTATE_DIE == m_eCurstate)
			return OBJ_DEAD;
		switch (m_eAnitype)
		{
		case Client::ANI_STOP:
			break;
		case Client::ANI_LOOP:
			break;
		case Client::ANI_BACKTOIDLE:
			m_eCurstate = OBJSTATE_IDLE;
			break;
		case Client::ANI_BACKTOTPSIDLE:
			m_eCurstate = OBJSTATE_TPSIDLE;
			break;
		case Client::ANI_END:
			break;
		default:
			break;
		}
			
	}
	return OBJ_NOEVENT;
}

void CFSM::Reset_Frame()
{
	m_pTextureCom[m_eCurstate]->Reset_Frame();
}

CFSM* CFSM::Create(void* pArg)
{
	CFSM* pInstance = new CFSM;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		delete pInstance;
		pInstance = nullptr;
		return nullptr;
	}

	return pInstance;
}

void CFSM::Destroy(CFSM* pInstance)
{
	if (nullptr != pInstance)
		delete pInstance;
	pInstance = nullptr;
}
