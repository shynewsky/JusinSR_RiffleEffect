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

class CUINumber final : public CImage
{
public:

private:
	CUINumber(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUINumber(const CUINumber& Prototype);
	virtual ~CUINumber() = default;
	
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
	_int m_iSkillNum;
	_float m_fSkillCoolTime = 0;

public:


private:
	DWORD m_dwTime = 0;

public:
	static CUINumber* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END