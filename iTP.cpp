// iTP.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "iTP.h"

#include "functions.h"
#include "notes.h"


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

static LRESULT CALLBACK Notes_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Controls
        int pad = 12, listW = 220, btnW = 80, btnH = 28;
        RECT rc; GetClientRect(hWnd, &rc); int W = rc.right, H = rc.bottom;

        gList = CreateWindowW(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER,
            pad, pad, listW, H - pad * 2 - btnH - 6, hWnd, (HMENU)ID_NOTES_LIST, GetModuleHandle(NULL), NULL);
        gEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            pad + listW + pad, pad, W - (listW + pad * 3), H - pad * 2 - btnH - 6, hWnd, (HMENU)ID_NOTES_EDIT, GetModuleHandle(NULL), NULL);

        gNew = CreateWindowW(L"BUTTON", L"New", WS_CHILD | WS_VISIBLE, pad, H - pad - btnH, btnW, btnH, hWnd, (HMENU)ID_NOTES_NEW, NULL, NULL);
        gSave = CreateWindowW(L"BUTTON", L"Save", WS_CHILD | WS_VISIBLE, pad + (btnW + 8), H - pad - btnH, btnW, btnH, hWnd, (HMENU)ID_NOTES_SAVE, NULL, NULL);
        gDel = CreateWindowW(L"BUTTON", L"Delete", WS_CHILD | WS_VISIBLE, pad + (btnW + 8) * 2, H - pad - btnH, btnW, btnH, hWnd, (HMENU)ID_NOTES_DELETE, NULL, NULL);
        gClose = CreateWindowW(L"BUTTON", L"Close", WS_CHILD | WS_VISIBLE, W - pad - btnW, H - pad - btnH, btnW, btnH, hWnd, (HMENU)ID_NOTES_CLOSE, NULL, NULL);

        Notes_LoadFromFile();
        Notes_RefreshList();
        return 0;
    }

    case WM_SIZE: {
        int pad = 12, listW = 220, btnW = 80, btnH = 28;
        int W = LOWORD(lParam), H = HIWORD(lParam);
        MoveWindow(gList, pad, pad, listW, H - pad * 2 - btnH - 6, TRUE);
        MoveWindow(gEdit, pad + listW + pad, pad, W - (listW + pad * 3), H - pad * 2 - btnH - 6, TRUE);
        MoveWindow(gNew, pad, H - pad - btnH, btnW, btnH, TRUE);
        MoveWindow(gSave, pad + (btnW + 8), H - pad - btnH, btnW, btnH, TRUE);
        MoveWindow(gDel, pad + (btnW + 8) * 2, H - pad - btnH, btnW, btnH, TRUE);
        MoveWindow(gClose, W - pad - btnW, H - pad - btnH, btnW, btnH, TRUE);
        return 0;
    }

    case WM_COMMAND: {
        WORD id = LOWORD(wParam), code = HIWORD(wParam);
        if (id == ID_NOTES_LIST && code == LBN_SELCHANGE) {
            int sel = (int)SendMessageW(gList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR) {
                int real = (int)SendMessageW(gList, LB_GETITEMDATA, sel, 0);
                Notes_LoadIntoUI(real);
            }
            return 0;
        }
        if (id == ID_NOTES_NEW) {
            int slot = Notes_FirstFree();
            if (slot >= 0) {
                gNotes[slot].used = TRUE; wcscpy_s(gNotes[slot].title, L"(untitled)"); gNotes[slot].body[0] = 0;
                Notes_SaveToFile(); Notes_RefreshList();
                int cnt = (int)SendMessageW(gList, LB_GETCOUNT, 0, 0);
                for (int i = 0; i < cnt; i++) if ((int)SendMessageW(gList, LB_GETITEMDATA, i, 0) == slot) { SendMessageW(gList, LB_SETCURSEL, i, 0); break; }
                Notes_LoadIntoUI(slot); SetFocus(gEdit);
            }
            return 0;
        }
        if (id == ID_NOTES_SAVE) { Notes_SaveCurrentFromUI(); return 0; }
        if (id == ID_NOTES_DELETE) {
            if (gCurrent >= 0) { gNotes[gCurrent].used = FALSE; gNotes[gCurrent].title[0] = 0; gNotes[gCurrent].body[0] = 0; gCurrent = -1; SetWindowTextW(gEdit, L""); Notes_SaveToFile(); Notes_RefreshList(); }
            return 0;
        }
        if (id == ID_NOTES_CLOSE) { ShowWindow(hWnd, SW_HIDE); return 0; }
        break;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX: {
        HDC hdc = (HDC)wParam;
        // SetBkColor(hdc, RGB(...));
        // return your brush handle:
        return (LRESULT)pale_brush;         // ✅ must return a brush for *CTLCOLOR* messages
    }



    case WM_CLOSE: ShowWindow(hWnd, SW_HIDE); return 0;

    default: return DefWindowProc(hWnd, msg, wParam, lParam);
    }
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
