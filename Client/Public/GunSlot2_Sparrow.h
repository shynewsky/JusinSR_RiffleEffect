#include "Client_Defines.h"
#include "Image.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CGunSlot2_Sparrow final : public CImage
{
public:
	enum GUN_STATE { OFF_NORMAL, ON_NORMAL, OFF_FEVER, ON_FEVER, GUN_END };

private:
	CGunSlot2_Sparrow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGunSlot2_Sparrow(const CGunSlot2_Sparrow& Prototype);
	virtual ~CGunSlot2_Sparrow() = default;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Render() override;

public:
	void Set_Gun(GUN_STATE eGun) { m_eGun = eGun; }

private:
	GUN_STATE m_eGun = GUN_END;

public:
	static CGunSlot2_Sparrow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END