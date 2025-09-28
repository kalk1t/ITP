// iTP.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "iTP.h"

#include "functions.h"



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    Notes_WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    load_bmps();


    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ITP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    WNDCLASSW nc = { 0 };
    nc.lpfnWndProc = Notes_WndProc;
    nc.hInstance = hInstance;
    nc.lpszClassName = L"NotesPopupClass";
    RegisterClassW(&nc);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ITP));

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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ITP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ITP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);
   int width = 900;
   int height = 900;
   int xC = (screenWidth - width) / 2;
   int yC = (screenHeight - height) / 2;
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      xC, yC, width,height, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

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

// Pale popup WndProc
static LRESULT CALLBACK Notes_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
         pale_brush = CreateSolidBrush(RGB(0, 0, 0)); 
         break;

    case WM_ERASEBKGND: {  
        HDC hdc = (HDC)wParam;
        RECT rc; GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, pale_brush);
        break;
    }

    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);  // hide instead of destroy (fast reopen)
        return 0;

    case WM_DESTROY:
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

static void ShowNotesPopup(HWND parent) {
    if (!notes_wnd) {
        notes_wnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW,                 // small tool-style title bar
            L"NotesPopupClass", L"Notes",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
            50, 200, 800, 700,
            parent, NULL, GetModuleHandle(NULL), NULL);

        SetLayeredWindowAttributes(notes_wnd, 0, (BYTE)(255 * 0.7), LWA_ALPHA);


        // Optional: center on parent
        RECT pr, wr; GetWindowRect(parent, &pr); GetWindowRect(notes_wnd, &wr);
        int w = wr.right - wr.left, h = wr.bottom - wr.top;
        int x = pr.left + ((pr.right - pr.left) - w) / 2;
        int y = pr.top + ((pr.bottom - pr.top) - h) / 2;
        SetWindowPos(notes_wnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
    ShowWindow(notes_wnd, SW_SHOW);
    SetForegroundWindow(notes_wnd);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    

    switch (message)
    {
    case WM_CREATE:
        {
        current_background = background_bmp;
        SetTimer(hWnd, 1, 5000, NULL);
        }
        break;
    case WM_TIMER:
        {
        if (wParam == 1) {
            current_background = (current_background == background_bmp) ? main_page_bmp : background_bmp;
            KillTimer(hWnd, 1);

            InvalidateRect(hWnd, NULL, TRUE);
            create_buttons(hWnd);
        }
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_BUTTON_SOCRATES:
            {
                ShowWindow(socrates_btn,SW_HIDE);
                current_background = socrates_background_bmp;
                InvalidateRect(hWnd, NULL, TRUE);
                ShowWindow(back_btn, SW_SHOW);
                ShowWindow(notes_btn, SW_SHOW);
            }
            break;
            case ID_BUTTON_BACK:
            {
                ShowWindow(back_btn, SW_HIDE);
                ShowWindow(notes_btn, SW_HIDE);
                current_background = main_page_bmp;
                ShowWindow(socrates_btn, SW_SHOW);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
            case ID_BUTTON_NOTES:
            {
                ShowNotesPopup(hWnd);
            }
            break;
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

            RECT rc;
            GetClientRect(hWnd, &rc);
            int cw = rc.right - rc.left;
            int ch = rc.bottom - rc.top;


            if (current_background) {
                BITMAP bm;
                GetObject(current_background, sizeof(bm), &bm);

                HDC hMemDC = CreateCompatibleDC(hdc);
                HGDIOBJ old = SelectObject(hMemDC, current_background);

                SetStretchBltMode(hdc, HALFTONE);
                StretchBlt(hdc, 0, 0, cw, ch, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

                SelectObject(hMemDC, old);
                DeleteDC(hMemDC);
            }
            else {
                FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
            }
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
