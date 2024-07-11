#pragma once
#include "GameObject.h"
#include "RenderingObject.h"

class CGauge abstract :
    public CRenderingObject
{
public:
	typedef struct
	{
		_float	fX;
		_float	fY;
		_float	fSizeX;
		_float	fSizeY;
		_float	fMaxValue;
		CGameObject* pTarget;
		_int	iFrameStart;
		_int	iFrameEnd;
		_float	fFrameSpeed;
	}GAUGE_DESC;

protected:
    CGauge(LPDIRECT3DDEVICE9 pGraphic_Device);
    CGauge(const CGauge& Prototype);
    virtual ~CGauge() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override = 0;
	virtual int Update(_float fTimeDelta) override = 0;
	virtual void Late_Update(_float fTimeDelta) override = 0;
	virtual HRESULT Render() override = 0;

protected:
	/* 직교투영 */
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_float	m_fX;
	_float	m_fY;
	_float	m_fSizeX;
	_float	m_fSizeY;
	_float	m_fCurValue;
	_float	m_fMaxValue;

protected:
	CGameObject* m_pTarget = { nullptr };
	void		 Adjust_GaugePos();
protected:
	virtual 	HRESULT SetUp_FSM() override { return S_OK; } //추상클래스 상속이라 만들어야됨
	
public: //상속할때 크리에이트 함수 잊지말기
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

