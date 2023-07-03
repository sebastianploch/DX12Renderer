// Copyright (c) 2023 Sebastian Ploch
#include "Resource.h"
#include "Core/Application.h"
#include "Core/RHI.h"

#include <ios>

// Global Variables:
HINSTANCE hInst; // current instance
LPCWSTR szTitle = L"DX12Renderer"; // The title bar text
LPCWSTR szWindowClass = L"MainWnd"; // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

namespace Console
{
	void ReopenBuffer(FILE* Buffer, const char* FileName, const char* Mode);
	void RedirectConsoleIO();
	void ReleaseConsole();
	void CreateNewConsole(SHORT MinLength);
	void AdjustConsoleBuffer(SHORT MinLength);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	MyRegisterClass(hInstance);

	// Initialize Console Window
	Console::CreateNewConsole(1024);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	std::unique_ptr<Application> application = std::make_unique<Application>();

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX12RENDERER));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Console::ReleaseConsole();
	return static_cast<int>(msg.wParam);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = nullptr;

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	RECT rect = {0, 0, static_cast<int32>(RHI::m_WindowInfo.m_Width), static_cast<int32>(RHI::m_WindowInfo.m_Height)};
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	const int32 width = rect.right - rect.left;
	const int32 height = rect.bottom - rect.top;

	RHI::m_WindowInfo.m_Window = CreateWindowW(szWindowClass,
	                                           szTitle,
	                                           WS_OVERLAPPEDWINDOW,
	                                           CW_USEDEFAULT,
	                                           CW_USEDEFAULT,
	                                           width,
	                                           height,
	                                           nullptr,
	                                           nullptr,
	                                           hInstance,
	                                           nullptr);

	if (!RHI::m_WindowInfo.m_Window)
	{
		return FALSE;
	}

	ShowWindow(RHI::m_WindowInfo.m_Window, nCmdShow);
	UpdateWindow(RHI::m_WindowInfo.m_Window);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Inspired from https://stackoverflow.com/questions/191842/how-do-i-get-console-output-in-c-with-a-windows-program
namespace Console
{
	void ReopenBuffer(FILE* Buffer, const char* FileName, const char* Mode)
	{
		FILE* stream;
		const errno_t reopenErrorCode = freopen_s(&stream, FileName, Mode, Buffer);
		if (reopenErrorCode != 0)
		{
			throw std::system_error(std::error_code(reopenErrorCode, std::system_category()), "Failed to reopen buffer");
		}

		const int setBufferErrorCode = setvbuf(Buffer, nullptr, _IONBF, 0);
		if (setBufferErrorCode != 0)
		{
			throw std::system_error(std::error_code(setBufferErrorCode, std::system_category()), "Failed to set buffering");
		}
	}

	void RedirectConsoleIO()
	{
		if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
		{
			ReopenBuffer(stdin, "CONIN$", "r");
		}

		if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
		{
			ReopenBuffer(stdout, "CONOUT$", "w");
		}

		if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
		{
			ReopenBuffer(stderr, "CONOUT$", "w");
		}

		std::ios::sync_with_stdio(true);
	}

	void ReleaseConsole()
	{
		ReopenBuffer(stdin, "NUL:", "r");
		ReopenBuffer(stdout, "NUL:", "w");
		ReopenBuffer(stderr, "NUL:", "w");

		if (!FreeConsole())
		{
			throw std::system_error(std::error_code(0, std::system_category()), "Failed to close console");
		}
	}

	void AdjustConsoleBuffer(SHORT MinLength)
	{
		const HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);

		if (consoleInfo.dwSize.Y < MinLength)
		{
			consoleInfo.dwSize.Y = MinLength;
		}

		SetConsoleScreenBufferSize(consoleHandle, consoleInfo.dwSize);
	}

	void CreateNewConsole(SHORT MinLength)
	{
		ReleaseConsole();

		if (AllocConsole())
		{
			AdjustConsoleBuffer(MinLength);
			RedirectConsoleIO();
		}
	}
}
