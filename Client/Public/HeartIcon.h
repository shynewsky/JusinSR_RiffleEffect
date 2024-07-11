#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Image.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CHeartIcon final : public CImage
{
public:
	enum HEART_STATE { HEART_PUMP, HEART_POP, HEART_END };

private:
	CHeartIcon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHeartIcon(const CHeartIcon& Prototype);
	virtual ~CHeartIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_HeartPop() 
	{ 
		m_eHeartState = HEART_POP; 
		m_dwTime = GetTickCount();
	}

private:
	HEART_STATE m_eHeartState = HEART_PUMP;
	DWORD m_dwTime = 0;

public:
	static CHeartIcon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END