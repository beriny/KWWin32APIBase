#include <Windows.h>

/*ウインドウプロシージャ（メッセージ処理）*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	/*メッセージ処理*/
	switch (msg)
	{
	case WM_DESTROY:
		/*ウインドウ破棄要求*/
		PostQuitMessage(0);/*スレッドへ終了要求を伝達*/
		break;
	default:
		/*規定のウインドウプロシージャ呼び出し*/
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return result;
}

/*メイン関数*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR pCmd, int nCmd)
{
	HRESULT hr;

	/*拡張ウインドウクラスの作成*/
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX)); /*未使用のパラメータは0*/
	wcex.cbSize = sizeof(WNDCLASSEX); /*構造体サイズ*/
	wcex.style = CS_HREDRAW | CS_VREDRAW; /*ウインドウスタイル（縦横サイズ変更で再描画）*/
	wcex.lpfnWndProc = WndProc; /*ウインドウプロシージャ設定*/
	wcex.hInstance = hInst; /*ハンドルインスタンス*/
	wcex.lpszClassName = "Win32App"; /*ウインドウクラス名*/

									 /*ウインドウクラスの登録*/
	if (RegisterClassEx(&wcex) == 0)
		return 0;

	/*ウインドウの作成*/
	HWND hwnd = CreateWindow(
		"Win32App", /*ウインドウクラス名*/
		"Win32 Application", /*タイトル*/
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, /*スタイル設定*/
		CW_USEDEFAULT, /*x座標*/
		CW_USEDEFAULT, /*y座標*/
		CW_USEDEFAULT, /*横幅*/
		CW_USEDEFAULT, /*立て幅*/
		NULL, /*親ウインドウ*/
		NULL, /*メニューバー*/
		hInst, /*インスタンス*/
		NULL /*WM_CRTEATEで渡すLPARAMの値*/);

	/*ウインドウ表示設定*/
	ShowWindow(hwnd, nCmd);
	UpdateWindow(hwnd);

	/*メッセージループ*/
	MSG msg;
	while ((hr = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (hr == -1) break;
		TranslateMessage(&msg); /*メッセージの変換*/
		DispatchMessage(&msg); /*メッセージの処理*/
	}

	return 0;
}