#pragma once

namespace Engine
{
	typedef struct
	{
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		bool			isWindowsed;
	}ENGINE_DESC;

	

	typedef struct 
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR2		vTexcoord;
	}VTXTEX;

	typedef struct
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vNormal;
		D3DXVECTOR2		vTexcoord;
	}VTXNORTEX;

	typedef struct
	{
		D3DXVECTOR3		vPosition;		
		D3DXVECTOR3		vTexcoord;
	}VTXCUBE;

	typedef struct
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vNormal;
		D3DXVECTOR3		vTexcoord;
	}VTXNORCUBE;

	typedef struct 
	{
		D3DXVECTOR3 vPosition;
		DWORD color;
	}VERTEXCOL;

}