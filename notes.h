#include <windows.h>
#include <stdio.h>

#define ID_NOTES_LIST    3001
#define ID_NOTES_EDIT    3002
#define ID_NOTES_NEW     3003
#define ID_NOTES_SAVE    3004
#define ID_NOTES_DELETE  3005
#define ID_NOTES_CLOSE   3006

#define MAX_NOTES 256

typedef struct {
    WCHAR title[128];
    WCHAR body[4096];
    BOOL  used;
} Note;

static HWND   gNotesWnd = NULL;
static HWND   gList = NULL, gEdit = NULL, gNew = NULL, gSave = NULL, gDel = NULL, gClose = NULL;
static Note   gNotes[MAX_NOTES];
static int    gCurrent = -1;


static void Notes_ClearAll(void) {
    for (int i = 0; i < MAX_NOTES; i++) gNotes[i].used = FALSE;
    gCurrent = -1;
}

static int Notes_FirstFree(void) {
    for (int i = 0; i < MAX_NOTES; i++) if (!gNotes[i].used) return i;
    return -1;
}

static void Notes_RefreshList(void) {
    SendMessageW(gList, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < MAX_NOTES; i++) {
        if (!gNotes[i].used) continue;
        int idx = (int)SendMessageW(gList, LB_ADDSTRING, 0, (LPARAM)gNotes[i].title);
        SendMessageW(gList, LB_SETITEMDATA, idx, i);
    }
}

static void Notes_LoadFromFile(void) {
    Notes_ClearAll();
    FILE* f = NULL;
    errno_t err = _wfopen_s(&f, L"notes.txt", L"rt, ccs=UTF-8");
    if (err != 0 || !f) {
        // handle error (file missing, etc.)
        return; // or your fallback
    }


    WCHAR line[4096], title[128] = L"", body[4096] = L"";
    int slot = -1; body[0] = 0;

    while (fgetws(line, 4096, f)) {
        size_t n = wcslen(line); while (n && (line[n - 1] == L'\n' || line[n - 1] == L'\r')) line[--n] = 0;
        if (wcsncmp(line, L"---", 3) == 0) {
            if (slot >= 0) {
                gNotes[slot].used = TRUE;
                wcsncpy_s(gNotes[slot].title, title[0] ? title : L"(untitled)", 127);
                gNotes[slot].title[127] = 0;
                wcsncpy_s(gNotes[slot].body, body, 4095);
                gNotes[slot].body[4095] = 0;
            }
            slot = Notes_FirstFree();
            title[0] = 0; body[0] = 0;
        }
        else if (wcsncmp(line, L"Title:", 6) == 0) {
            const WCHAR* p = line + 6; while (*p == L' ') ++p;
            wcsncpy_s(title, p, 127); title[127] = 0;
        }
        else {
            if (body[0]) wcsncat_s(body, L"\r\n", 4095 - wcslen(body));
            wcsncat_s(body, line, 4095 - wcslen(body));
        }
    }
    if (slot >= 0) {
        gNotes[slot].used = TRUE;
        wcsncpy_s(gNotes[slot].title, title[0] ? title : L"(untitled)", 127);
        gNotes[slot].title[127] = 0;
        wcsncpy_s(gNotes[slot].body, body, 4095);
        gNotes[slot].body[4095] = 0;
    }
    fclose(f);
}

static void Notes_SaveToFile(void) {
    FILE* f = NULL;
    errno_t err = _wfopen_s(&f, L"notes.txt", L"wt, ccs=UTF-8");
    if (err != 0 || !f) {
        // handle error
        return;
    }

    if (!f) return;
    for (int i = 0; i < MAX_NOTES; i++) {
        if (!gNotes[i].used) continue;
        fwprintf(f, L"---\nTitle: %s\n", gNotes[i].title[0] ? gNotes[i].title : L"(untitled)");
        fwprintf(f, L"%s\n", gNotes[i].body[0] ? gNotes[i].body : L"");
    }
    fclose(f);
}


static void Notes_LoadIntoUI(int realIndex) {
    if (realIndex < 0 || !gNotes[realIndex].used) { SetWindowTextW(gEdit, L""); gCurrent = -1; return; }
    SetWindowTextW(gEdit, gNotes[realIndex].body);
    gCurrent = realIndex;
}

static void Notes_SaveCurrentFromUI(void) {
    if (gCurrent < 0) return;
    WCHAR text[4096]; GetWindowTextW(gEdit, text, 4096);
    wcsncpy_s(gNotes[gCurrent].body, text, 4095); gNotes[gCurrent].body[4095] = 0;

    // Title = first line (fallback if empty)
    const WCHAR* p = text; while (*p == L' ' || *p == L'\r' || *p == L'\n') ++p;
    WCHAR t[128] = L""; int k = 0; while (*p && *p != L'\r' && *p != L'\n' && k < 127) t[k++] = *p++; t[k] = 0;
    if (!t[0]) wcscpy_s(t, L"(untitled)");
    wcsncpy_s(gNotes[gCurrent].title, t, 127); gNotes[gCurrent].title[127] = 0;

    Notes_SaveToFile();
    Notes_RefreshList();
    // reselect current
    int count = (int)SendMessageW(gList, LB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; i++) if ((int)SendMessageW(gList, LB_GETITEMDATA, i, 0) == gCurrent) { SendMessageW(gList, LB_SETCURSEL, i, 0); break; }
}

