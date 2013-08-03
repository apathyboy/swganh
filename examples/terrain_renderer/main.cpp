// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>

#include <fstream>
#include <vector>

#include <glm/glm.hpp>

#include "swganh/byte_buffer.h"
#include "swganh/tre/visitors/terrain/terrain_visitor.h"
#include "swganh/tre/iff/iff.h"

typedef struct _RGNDATAHEADER {
	DWORD dwSize;
	DWORD iType;
	DWORD nCount;
	DWORD nRgnSize;
	RECT  rcBound;
} RGNDATAHEADER, *PRGNDATAHEADER;

typedef struct _RGNDATA {
	RGNDATAHEADER rdh;
	char          Buffer[1];
} RGNDATA, *PRGNDATA;

#include <d3d9.h>

// define the screen resolution
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 2048

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    // the pointer to the vertex buffer

// function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);    // closes Direct3D and releases memory
void init_graphics(void);    // 3D declarations

struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		"WindowClass",
		"Our Direct3D Program",
		WS_OVERLAPPEDWINDOW,
		0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		render_frame();
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	init_graphics();    // call the function to initialize the triangle
}


// this is the function used to render a single frame
void render_frame(void)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();

	// select which vertex format we are using
	d3ddev->SetFVF(CUSTOMFVF);

	// select the vertex buffer to display
	//d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

	// copy the vertex buffer to the back buffer
	//d3ddev->DrawPrimitive(D3DPT_POINTLIST, 0, 1);
	
	std::ifstream in("testdata/terrain/tatooine.trn", std::ios::in | std::ios::binary);

	if (!in.is_open())
	{
		return;
	}

	std::vector<char> tmp { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
	swganh::ByteBuffer data(reinterpret_cast<unsigned char*>(tmp.data()), tmp.size());

	auto terrain_visitor = std::make_shared<swganh::tre::TerrainVisitor>();

	swganh::tre::iff_file::loadIFF(data, terrain_visitor);

	const auto& wmap = terrain_visitor->GetWmap();
	std::vector<CUSTOMVERTEX> vertices2;

	uint32_t map_width = terrain_visitor->GetMapWidth();
	uint32_t map_height = terrain_visitor->GetMapHeight();

	for (unsigned int i = 0; i < map_height; ++i)
	{
		for (unsigned int j = 0; j < map_width; ++j)
		{
			unsigned char c = wmap.at((i * map_width) + j);
			CUSTOMVERTEX v = { (float) j, (float) i, 1.0f, 1.0, D3DCOLOR_XRGB(c, c, c) };
			vertices2.push_back(v);
			//total += base::read(file, x);
#if 0
			std::cout << (unsigned int) x << " ";
#endif
		}
	}

	d3ddev->DrawPrimitiveUP(D3DPT_POINTLIST,        //PrimitiveType
		vertices2.size(),                //PrimitiveCount
		vertices2.data(),                   //pVertexStreamZeroData
		sizeof(CUSTOMVERTEX));  //VertexStreamZeroStride

	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	v_buffer->Release();    // close and release the vertex buffer
	d3ddev->Release();    // close and release the 3D device
	d3d->Release();    // close and release Direct3D
}


// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
	std::ifstream in("testdata/terrain/tatooine.trn", std::ios::in | std::ios::binary);

	if (!in.is_open())
	{
		return;
	}

	std::vector<char> tmp { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
	swganh::ByteBuffer data(reinterpret_cast<unsigned char*>(tmp.data()), tmp.size());

	auto terrain_visitor = std::make_shared<swganh::tre::TerrainVisitor>();

	swganh::tre::iff_file::loadIFF(data, terrain_visitor);

	const auto& wmap = terrain_visitor->GetWmap();
	std::vector<CUSTOMVERTEX> vertices2;

	uint32_t map_width = terrain_visitor->GetMapWidth();
	uint32_t map_height = terrain_visitor->GetMapHeight();

	for (unsigned int i = 0; i < map_height; ++i)
	{
		for (unsigned int j = 0; j < map_width; ++j)
		{
			CUSTOMVERTEX v = { (float) j, (float) i, 1.0f, 1.0, D3DCOLOR_XRGB(0, 255, 0) };
			vertices2.push_back(v);
			//total += base::read(file, x);
#if 0
			std::cout << (unsigned int) x << " ";
#endif
		}
	}

	// create a vertex buffer interface called v_buffer
	d3ddev->CreateVertexBuffer(vertices2.size() * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);
	
	VOID* pVoid;    // a void pointer
	
	// lock v_buffer and load the vertices into it
	v_buffer->Lock(0, 0, (void**) &pVoid, 0);
	memcpy(pVoid, vertices2.data(), vertices2.size() * sizeof(CUSTOMVERTEX));
	v_buffer->Unlock();


	//// create the vertices using the CUSTOMVERTEX struct
	//std::vector<CUSTOMVERTEX> vertices3 =
	//{
	//	{ 400.0f, 62.5f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 0, 255), },
	//	{ 650.0f, 500.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 255, 0), },
	//	{ 150.0f, 500.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(255, 0, 0), },
	//};
	//
	//// create a vertex buffer interface called v_buffer
	//d3ddev->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
	//	0,
	//	CUSTOMFVF,
	//	D3DPOOL_MANAGED,
	//	&v_buffer,
	//	NULL);
	//
	//VOID* pVoid;    // a void pointer
	//
	//// lock v_buffer and load the vertices into it
	//v_buffer->Lock(0, 0, (void**) &pVoid, 0);
	//memcpy(pVoid, vertices3.data(), sizeof(CUSTOMVERTEX) * vertices3.size());
	//v_buffer->Unlock();


	//// create the vertices using the CUSTOMVERTEX struct
	//CUSTOMVERTEX vertices [] =
	//{
	//	{ 400.0f, 62.5f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 0, 255), },
	//	{ 650.0f, 500.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 255, 0), },
	//	{ 150.0f, 500.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(255, 0, 0), },
	//};
	//
	//// create a vertex buffer interface called v_buffer
	//d3ddev->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
	//	0,
	//	CUSTOMFVF,
	//	D3DPOOL_MANAGED,
	//	&v_buffer,
	//	NULL);
	//
	//VOID* pVoid;    // a void pointer
	//
	//// lock v_buffer and load the vertices into it
	//v_buffer->Lock(0, 0, (void**) &pVoid, 0);
	//memcpy(pVoid, vertices, sizeof(vertices));
	//v_buffer->Unlock();
}
