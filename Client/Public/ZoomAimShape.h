#pragma once
#include "RenderingObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CZoomAimShape :
    public CRenderingObject
{
public:
	typedef struct
	{
		_float	fX;
		_float	fY;
		_float	fSizeX;
		_float	fSizeY;
	}AIM_DESC;

private:
	CZoomAimShape(LPDIRECT3DDEVICE9 pGraphic_Device);
	CZoomAimShape(const CZoomAimShape& Prototype);
	virtual ~CZoomAimShape() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Reset_Frame();
	void Set_OrthoLH();
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	virtual HRESULT	Add_RenderObject(class CGameObject* pRenderObject) override;

	/* 직교투영 */
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_float	m_fX;
	_float	m_fY;
	_float	m_fSizeX;
	_float	m_fSizeY;
	_float  m_fTheta = {0.f};

	bool m_bMouseOver = false;

protected:
	virtual 	HRESULT SetUp_FSM() { return S_OK; }

private:
	void Roll(_float _fTimeDelta);

public:
	static CZoomAimShape* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END