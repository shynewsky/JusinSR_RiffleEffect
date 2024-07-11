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

class CUIImmortal final : public CImage
{
public:

private:
	CUIImmortal(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIImmortal(const CUIImmortal& Prototype);
	virtual ~CUIImmortal() = default;
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Set_Variables(void* pArg);
	void Set_OrthoLH();
	LEVELID m_eCurLevel;
	_float m_fImmotalCount = 0;

private:
	DWORD m_dwTime = 0;

public:
	static CUIImmortal* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END