#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;

END

BEGIN(Client)

class CUIOrthogonal abstract : public CGameObject
{
public:
	typedef struct
	{
		_float3			vPos;
		_float			fSizeX;
		_float			fSizeY;
		_bool			bCull = true;
	}Orthogonal_DESC;

protected:
	CUIOrthogonal(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIOrthogonal(const CUIOrthogonal& Prototype);
	virtual ~CUIOrthogonal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_RenderState(bool _state) { m_bRenderState = _state; }


protected:
	void Set_OrthoLH(CTransform* pTransform);
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

protected:
	_float3 m_vWorldPos;
	_float4x4				m_ViewMatrix, m_ProjMatrix, m_prevProject;
	_float	m_fX = 0.f;
	_float	m_fY = 0.f;
	_float	m_fZ = 0.f;
	_float	m_fSizeX;
	_float	m_fSizeY;
private:
	bool					m_bRenderState = false;
	/* 직교투영 */


	bool	m_bCull;


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END