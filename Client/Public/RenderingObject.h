#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"
#include "FSM.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CRenderingObject abstract:
    public CBlendObject
{
public:
	typedef struct
	{
		_float3 vPos;
		_float3 vLook;
		_float fCX;
		_float fCY;
	}RENDERINGOBJECT_DESC;
protected:
	CRenderingObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRenderingObject(const CRenderingObject& Prototype);
	virtual ~CRenderingObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	virtual void Reset_Frame() {};
	virtual void Set_State(CFSM::OBJSTATE _eState) {};
	virtual HRESULT	Add_RenderObject(class CGameObject* pRenderObject);
	void Set_TargetTransform(CTransform* _pTransform);

	//for parts
protected:
	void Set_OrthoLH();

	_float m_fX = {};
	_float m_fY = {};
	_float m_fZ = {};
	_float m_fSizeX, m_fSizeY = {};


	_float4x4 m_ViewMatrix, m_ProjMatrix, m_PrevView, m_PrevProj;


protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pTargetTransform = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CFSM*		m_pFSM;

	virtual 	HRESULT SetUp_FSM() = 0;
	_float3 m_vLook;
	_float	m_fCX;
	_float	m_fCY;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END