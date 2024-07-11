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

class CLogoBT_Quit final : public CButton
{
private:
	CLogoBT_Quit(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoBT_Quit(const CLogoBT_Quit& Prototype);
	virtual ~CLogoBT_Quit() = default;

public:
	static CLogoBT_Quit* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END