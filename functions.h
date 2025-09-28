#pragma once

void load_bmps() {
    background_bmp = (HBITMAP)LoadImageA(0, "acropolis.bmp", IMAGE_BITMAP, 900, 900, LR_LOADFROMFILE);
    main_page_bmp = (HBITMAP)LoadImageA(0, "main_page.bmp", IMAGE_BITMAP, 900, 900, LR_LOADFROMFILE);
    socrates_btn_bmp= (HBITMAP)LoadImageA(0, "socrates_btn.bmp", IMAGE_BITMAP, 120, 40, LR_LOADFROMFILE);
    socrates_background_bmp = (HBITMAP)LoadImageA(0, "socrates_background.bmp", IMAGE_BITMAP, 900, 900, LR_LOADFROMFILE);
    notes_btn_bmp = (HBITMAP)LoadImageA(0, "notes_btn.bmp", IMAGE_BITMAP, 60, 30, LR_LOADFROMFILE);
    back_btn_bmp = (HBITMAP)LoadImageA(0, "back_btn.bmp", IMAGE_BITMAP, 60, 30, LR_LOADFROMFILE);
}

void create_buttons(HWND hWnd) {
    socrates_btn = CreateWindowW(L"BUTTON", NULL,
        WS_VISIBLE | WS_CHILD | BS_BITMAP,
        20, 20, 120, 40, hWnd, (HMENU)ID_BUTTON_SOCRATES,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    SendMessageW(socrates_btn, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)socrates_btn_bmp);
    SendMessageW(socrates_btn, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0); //hides focus

    back_btn = CreateWindowW(L"BUTTON", NULL,
        WS_VISIBLE | WS_CHILD | BS_BITMAP,
        400, 400, 60, 30, hWnd, (HMENU)ID_BUTTON_BACK,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    SendMessageW(back_btn, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)back_btn_bmp);
    SendMessageW(back_btn, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0); //hides focus
    ShowWindow(back_btn, SW_HIDE);

    notes_btn= CreateWindowW(L"BUTTON", NULL,
        WS_VISIBLE | WS_CHILD | BS_BITMAP,
        400, 360, 60, 30, hWnd, (HMENU)ID_BUTTON_NOTES,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    SendMessageW(notes_btn, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)notes_btn_bmp);
    SendMessageW(notes_btn, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0); //hides focus
    ShowWindow(notes_btn, SW_HIDE);
}