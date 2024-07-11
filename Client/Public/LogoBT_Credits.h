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

class CLogoBT_Credits final : public CButton
{
private:
	CLogoBT_Credits(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoBT_Credits(const CLogoBT_Credits& Prototype);
	virtual ~CLogoBT_Credits() = default;

public:
	static CLogoBT_Credits* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END