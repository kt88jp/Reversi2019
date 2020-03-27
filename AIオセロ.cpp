// オセロAI（Win 32）.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "AIオセロ.h"
#include "Game.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

Game gamedata;

HMENU hmenu;

map<int, pair<int, Game::EvalType>> menuid_to_bw = {
	{IDM_BPlayer, make_pair(0, Game::EvalType::HUMAN)},
	{IDM_BRandom, make_pair(0, Game::EvalType::RANDOM)},
	{IDM_BGreedy, make_pair(0, Game::EvalType::GREEDY)},
	{IDM_BEvaluation, make_pair(0, Game::EvalType::FIELDEVALUATION)},
	{IDM_WPlayer, make_pair(1, Game::EvalType::HUMAN)},
	{IDM_WRandom, make_pair(1, Game::EvalType::RANDOM)},
	{IDM_WGreedy, make_pair(1, Game::EvalType::GREEDY)},
	{IDM_WEvaluation, make_pair(1, Game::EvalType::FIELDEVALUATION)},

};

map <pair<int, Game::EvalType>, int> bw_to_menuid;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: ここにコードを挿入してください。

	// グローバル文字列を初期化しています。
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_AI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AI));

	MSG msg;

	// メイン メッセージ ループ:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_AI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


void setmenu() {
	for (auto& id : menuid_to_bw) {
		CheckMenuItem(hmenu, id.first, MFS_UNCHECKED);
	}
	CheckMenuItem(hmenu, bw_to_menuid[make_pair(0, gamedata.evaltype[0])], MFS_CHECKED);
	CheckMenuItem(hmenu, bw_to_menuid[make_pair(1, gamedata.evaltype[1])], MFS_CHECKED);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	HWND hWnd = CreateWindowEx(WS_EX_COMPOSITED , szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	for (auto& it : menuid_to_bw) {
		bw_to_menuid[it.second] = it.first;
	}

	hmenu = GetMenu(hWnd);
	gamedata.sethWnd(hWnd);
	setmenu();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}



//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウの描画
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_AI32791:
			gamedata.waittime = 0;
			gamedata.isWait = false;
			CheckMenuItem(hmenu, ID_AI32791, MFS_CHECKED);
			CheckMenuItem(hmenu, ID_AI32792, MFS_UNCHECKED);
			break;
		case ID_AI32792:
			gamedata.waittime = 5;
			gamedata.isWait = true;
			CheckMenuItem(hmenu, ID_AI32792, MFS_CHECKED);
			CheckMenuItem(hmenu, ID_AI32791, MFS_UNCHECKED);
		case ID_32780:
			gamedata.skip = !gamedata.skip;
			CheckMenuItem(hmenu, ID_32780, gamedata.skip ? MFS_UNCHECKED : MFS_CHECKED);
			break;
		case ID_32793:
			gamedata.init();
			break;
		case ID_Menu32795:
			gamedata.startAutobattle(10, hmenu);
			break;
		case ID_32796:
			gamedata.startAutobattle(100, hmenu);
			break;
		case ID_32797:
			gamedata.startAutobattle(1000, hmenu);
			break;
		case ID_32798:
			CheckMenuItem(hmenu, ID_32798, MFS_CHECKED);
			gamedata.AIdepth[0] = 1;
			break;
		case ID_32799:
			CheckMenuItem(hmenu, ID_32799, MFS_CHECKED);
			CheckMenuItem(hmenu, ID_32798, MFS_UNCHECKED);
			gamedata.AIdepth[0] = 2;
			break;
		case ID_32801:
			CheckMenuItem(hmenu, ID_32801, MFS_CHECKED);
			CheckMenuItem(hmenu, ID_32802, MFS_UNCHECKED);
			gamedata.AIdepth[1] = 1;
			break;
		case ID_32802:
			CheckMenuItem(hmenu, ID_32802, MFS_CHECKED);
			CheckMenuItem(hmenu, ID_32801, MFS_UNCHECKED);
			gamedata.AIdepth[1] = 2;
			break;

		default:
			auto it = menuid_to_bw.find(wmId);
			if (it != menuid_to_bw.end()) {
				gamedata.evaltype[it->second.first] = it->second.second;
				setmenu();
			}


			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CREATE:
		SetTimer(hWnd, 1, 50, NULL);
		break;
	case WM_TIMER:
		gamedata.loop();
		//UpdateWindow(hwnd);
		gamedata.invalidate();
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: HDC を使用する描画コードをここに追加してください...
		gamedata.draw(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		gamedata.waitcounter = 0;
		gamedata.mousedown(x, y);
		//gamedata.invalidate();
	}
	break;
	case WM_KEYDOWN:
		gamedata.keyfunc(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
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
