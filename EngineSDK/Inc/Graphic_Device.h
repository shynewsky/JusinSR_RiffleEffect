#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	// 장치 초기화 과정

	// 1. 장치를 조사할 객체를 먼저 생성
	// 2. 장치를 조사하도록 지시(지원 수준 파악)
	// 3. 장치를 제어할 객체를 생성

	HRESULT		Init_Device(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	void		Render_Begin();
	void		Render_End(HWND hWnd = nullptr);

public:
	LPDIRECT3DDEVICE9	Get_Device() { return m_pDevice; }
	LPD3DXSPRITE		Get_Sprite() { return m_pSprite; }
	LPD3DXFONT			Get_Font() { return m_pFont; }
private:
	void		Set_Parameters(D3DPRESENT_PARAMETERS& d3dpp, const ENGINE_DESC& EngineDesc);

private:
	LPDIRECT3D9			m_pSDK;		// 1번에 해당하는 객체
	LPDIRECT3DDEVICE9	m_pDevice;	// 3번에 해당하는 객체
	LPD3DXSPRITE		m_pSprite;	// dx 상에서 2d 이미지 출력을 담당하는 객체
	LPD3DXFONT			m_pFont;

public:
	static CGraphic_Device* Create(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	virtual void Free() override;
};

END