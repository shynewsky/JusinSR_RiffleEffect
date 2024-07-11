#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CButton abstract : public CGameObject
{
public:
	typedef struct
	{
		_float	fX;		// 버튼 중점 좌표
		_float	fY;
		_float	fSizeX; // 버튼 가로세로 길이
		_float	fSizeY;
		_uint	iImgNormal;		// 어떤 이벤트도 없을떄 텍스쳐 인덱스
		_uint	iImgMouseOver;	// 마우스오버했을떄 텍스쳐 인덱스

		LEVELID	eLevelIndex;	// 텍스쳐 컴포넌트 레벨인덱스
		const _tchar* tFrameTextureTag;	// 이미지 택스쳐 태그

	}BUTTON_DESC;

protected:
	CButton(LPDIRECT3DDEVICE9 pGraphic_Device);
	CButton(const CButton& Prototype);
	virtual ~CButton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual bool Get_MouseOver() { return m_bMouseOver; } // --> 클래스 내에서 화면전환

protected:
	void Set_Variables(void* pArg);
	void Set_OrthoLH();
	void ChecK_MouseOver(); // --> 이거만 버튼 컴포넌트로 이동 (소용없을듯)
	virtual HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

protected:
	CTexture*				m_pTextureCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

protected:
	/* 직교투영 */
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	BUTTON_DESC				m_tButtonInfo = {};

	bool m_bMouseOver = false;
	bool m_bSoundOn = false;

public:
	//static CButton* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) = 0 ;
	virtual void Free() override;
};

END