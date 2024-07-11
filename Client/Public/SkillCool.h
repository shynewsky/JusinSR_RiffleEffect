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

class CSkillCool final : public CImage
{
public:

private:
	CSkillCool(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkillCool(const CSkillCool& Prototype);
	virtual ~CSkillCool() = default;
	
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
	int m_iSkillNum;
	_float m_fSkillCoolTime = 0;

public:


private:
	DWORD m_dwTime = 0;

public:
	static CSkillCool* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END