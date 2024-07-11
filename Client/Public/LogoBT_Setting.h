#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Button.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLogoBT_Setting final : public CButton
{
private:
	CLogoBT_Setting(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoBT_Setting(const CLogoBT_Setting& Prototype);
	virtual ~CLogoBT_Setting() = default;

public:
	static CLogoBT_Setting* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END