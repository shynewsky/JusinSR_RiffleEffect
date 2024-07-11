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

class CLogoBT_Practice final : public CButton
{
private:
	CLogoBT_Practice(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoBT_Practice(const CLogoBT_Practice& Prototype);
	virtual ~CLogoBT_Practice() = default;

public:
	static CLogoBT_Practice* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END