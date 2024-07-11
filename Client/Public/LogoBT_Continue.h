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

class CLogoBT_Continue final : public CButton
{
private:
	CLogoBT_Continue(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLogoBT_Continue(const CLogoBT_Continue& Prototype);
	virtual ~CLogoBT_Continue() = default;

public:
	static CLogoBT_Continue* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END