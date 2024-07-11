#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

BEGIN(Client)



class CFence final : public CBlendObject
{
public:
	typedef struct
	{
		_float	fRadiusX = 0.5f;
		_float	fRadiusY = 0.5f;
		_float	fRadiusZ = 0.5f;

	}FENCE_DESC;


private:
	CFence(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFence(const CFence& Prototype);
	virtual ~CFence() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	void Set_Defense(_bool bDefense) { m_bDefense = bDefense;}
private:
	CTexture*				m_pTextureCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };


	_float4x4				m_CollisionMatrix;
	_bool					m_bDefense = false;


private:
	HRESULT Ready_Components(void* pArg);


public:
	static CFence* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END