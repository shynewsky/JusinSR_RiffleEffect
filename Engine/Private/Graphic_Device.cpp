#include "..\Public\Graphic_Device.h"

CGraphic_Device::CGraphic_Device() 
	: m_pDevice(nullptr)
	, m_pSDK(nullptr)
	, m_pSprite(nullptr)
{
}

HRESULT CGraphic_Device::Init_Device(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice)
{
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9		DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// GetDeviceCaps : 장치에 대한 지원 수준을 조사하여 값을 얻어오는 함수
	// D3DADAPTER_DEFAULT : 정보를 얻으려는 기본 그래픽 카드를 의미
	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		MSG_BOX(L"GetDeviceCaps Failed");
		return E_FAIL;
	}

	// 버텍스 프로세싱 = 정점 변환 + 조명(명암) 연산

	DWORD		vp(0);

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;


	D3DPRESENT_PARAMETERS		d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	Set_Parameters(d3dpp, EngineDesc);

	// CreateDevice : 장치를 제어할 객체를 생성하는 함수
	// CreateDevice(어떤 그래픽 카드를 제어할 것인가, 어떤 정보로 장치에 접근할 것인가, 장치를 사용할 윈도우 핸들, 동작 방식, 사용 환경, 생성할 객체를 담을 포인터)
	
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, EngineDesc.hWnd, vp, &d3dpp, &m_pDevice)))
	{
		MSG_BOX(L"CreateDevice Failed");
		return E_FAIL;
	}

	// 스프라이트

	if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
	{
		MSG_BOX(L"CreateSprite Failed");
		return E_FAIL;
	}

	D3DXFONT_DESCW		tFontInfo;
	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));

	tFontInfo.Height = 40;
	tFontInfo.Width = 20;
	tFontInfo.Weight = FW_HEAVY;
	tFontInfo.CharSet = HANGEUL_CHARSET;
	lstrcpy(tFontInfo.FaceName, L"온글잎 류뚱체 Regular");

	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont)))
	{
		MSG_BOX(L"CreateFontIndirect Failed");
		return E_FAIL;
	}

	*ppDevice = m_pDevice;
	Safe_AddRef(m_pDevice);

	

	return S_OK;
}

void CGraphic_Device::Render_Begin()
{
	m_pDevice->Clear(0,			// 렉트의 개수
					nullptr,	// 렉트 배열의 첫 번째 주소
					D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
					D3DCOLOR_ARGB(255, 0, 0, 0), // 백 버퍼의 색상
					1.f,	// 깊이 버퍼의 초기화 값
					0);		// 스텐실 버퍼의 초기화 값

	m_pDevice->BeginScene();	// 여기서부터 후면 버퍼에 그리기 시작
}

void CGraphic_Device::Render_End(HWND hWnd)
{	
	m_pDevice->EndScene();		// 후면 버퍼에 그리기가 끝난 시점

	// 후면 버퍼의 surface와 전면 버퍼의 surface를 교환
	m_pDevice->Present(nullptr, nullptr, hWnd, nullptr);

	// 1, 2 : D3DSWAPEFFECT_COPY 여야 동작
	// 3 : null인 경우 Set_Parameters 함수 안에 지정한 핸들로 자동 삽입
	// 4 : D3DSWAPEFFECT_COPY 여야 동작
}

void CGraphic_Device::Set_Parameters(D3DPRESENT_PARAMETERS & d3dpp, const ENGINE_DESC& EngineDesc)
{
	d3dpp.BackBufferWidth  = EngineDesc.iWinSizeX;
	d3dpp.BackBufferHeight = EngineDesc.iWinSizeY;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount  = 1;
	
	d3dpp.MultiSampleType    = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	// D3DSWAPEFFECT_DISCARD : 스왑 체인 방식
	// D3DSWAPEFFECT_FLIP : 버퍼 하나로 뒤집으면서 사용하는 방식
	// D3DSWAPEFFECT_COPY : 더블 버퍼링과 유사한 복사 방식
	
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// 장치를 사용할 윈도우 핸들
	d3dpp.hDeviceWindow = EngineDesc.hWnd;

	d3dpp.Windowed = TRUE;	// 창 모드 또는 전체화면 모드 실행 선택(TRUE : 창 모드)

	d3dpp.EnableAutoDepthStencil = TRUE;		// 스텐실 버퍼와 깊이 버퍼의 관리 상태
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	

	// 전체 화면 모드 시 모니터 재생률을 토대로 수행
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	// 재생률과 시연의 간격 설정
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

CGraphic_Device * CGraphic_Device::Create(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice)
{
	CGraphic_Device*		pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Init_Device(EngineDesc, ppDevice)))
	{
		MSG_BOX(TEXT("Failed to Created : CGraphic_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);

	if (0 != Safe_Release(m_pDevice))
		MSG_BOX(TEXT("Failed to Release Com-Instance"));

	Safe_Release(m_pSDK);

	__super::Free();

}
