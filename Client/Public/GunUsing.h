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

class CGunUsing final : public CImage
{
public:
	enum GUN_STATE {
		GUN1_NORMAL, GUN1_FEVER,
		GUN2_NORMAL, GUN2_FEVER,
		GUN3_NORMAL, GUN3_FEVER,
		GUN_END
	};

private:
	CGunUsing(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGunUsing(const CGunUsing& Prototype);
	virtual ~CGunUsing() = default;

public:
	virtual HRESULT Render() override;

public:
	void Set_Gun(GUN_STATE eGun) { m_eGun = eGun; }

public:
	GUN_STATE m_eGun = GUN_END;

public:
	static CGunUsing* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END