#pragma once
#include <Windows.h>
#include <string>
#include <sstream>

class KWWin32APIBase
{
public:

	KWWin32APIBase(HINSTANCE hInst, std::string title) :
		hinstance(hInst),
		title_name(title),
		width(CW_USEDEFAULT),
		height(CW_USEDEFAULT)
	{
		std::ostringstream ss;
		ss << "wnd_class_" << unique_number++;
		wndclass_name = ss.str();

		pre_initialize();
		PrepareWindowClass();
		EnableWindow(SW_SHOWNORMAL);
		post_initialize();

		LPRECT rect;
		GetClientRect(hwnd, rect);
		width = static_cast<UINT>(rect->right);
		height = static_cast<UINT>(rect->bottom);
	}

	KWWin32APIBase(HINSTANCE hInst, std::string title, int nCmd) :
		hinstance(hInst),
		title_name(title),
		width(CW_USEDEFAULT),
		height(CW_USEDEFAULT)
	{
		std::ostringstream ss;
		ss << "wnd_class_" << unique_number++;
		wndclass_name = ss.str();

		pre_initialize();
		PrepareWindowClass();
		EnableWindow(nCmd);
		post_initialize();

		RECT rect;
		GetWindowRect(hwnd, &rect);
		resize(static_cast<UINT>(rect.right - rect.left),
			static_cast<UINT>(rect.bottom - rect.top));
	}

	KWWin32APIBase(HINSTANCE hInst, std::string title, int W, int H) :
		hinstance(hInst),
		title_name(title),
		width(W),
		height(H)
	{
		std::ostringstream ss;
		ss << "wnd_class_" << unique_number++;
		wndclass_name = ss.str();

		pre_initialize();
		PrepareWindowClass();
		EnableWindow(SW_SHOWNORMAL);
		post_initialize();
	}

	KWWin32APIBase(HINSTANCE hInst, std::string title, int nCmd, int W, int H) :
		hinstance(hInst),
		title_name(title),
		width(W),
		height(H)
	{
		std::ostringstream ss;
		ss << "wnd_class_" << unique_number++;
		wndclass_name = ss.str();

		pre_initialize();
		PrepareWindowClass();
		EnableWindow(nCmd);
		post_initialize();
	}

	virtual ~KWWin32APIBase() {}

	void Run(int loop_type) {
		if (loop_type == 0)
			MessageLoop();
		else
			MainLoop();
	}

protected:
	UINT width;
	UINT height;
	HINSTANCE hinstance;
	HWND hwnd;

	virtual void pre_initialize() {} /*ウインドウ作成前*/
	virtual void post_initialize() {} /*ウインドウ作成後*/
	virtual void draw() {} /*描画（WM_PAINT）*/
	virtual void resize(int W, int H) /*リサイズ（WM_SIZE）*/
	{
		width = W;
		height = H;
	}
	virtual void pre_finish() {} /*ウインドウ破棄（WM_DESTOROY）*/
	virtual void post_finish() {} /*終了（WM_QUIT）*/
	virtual void update() {} /*更新（メインループ内非メッセージ処理）*/

private:
	static int unique_number;
	std::string wndclass_name;
	std::string title_name;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	/*ウインドウクラスの作成と登録*/
	ATOM PrepareWindowClass() {
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(WNDCLASSEX)); /*未使用のパラメータは0*/
		wcex.cbSize = sizeof(WNDCLASSEX); /*構造体サイズ*/
		wcex.style = CS_HREDRAW | CS_VREDRAW; /*ウインドウスタイル（縦横サイズ変更で再描画）*/
		wcex.lpfnWndProc = WndProc; /*ウインドウプロシージャ設定*/
		wcex.hInstance = hinstance; /*ハンドルインスタンス*/
		wcex.lpszClassName = wndclass_name.c_str(); /*ウインドウクラス名*/

		return RegisterClassEx(&wcex);
	}

	/*ウインドウの作成と表示設定*/
	BOOL EnableWindow(int nCmdShow) {
		BOOL ret = TRUE;

		hwnd = CreateWindow(
			wndclass_name.c_str(), /*ウインドウクラス名*/
			title_name.c_str(), /*タイトル*/
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, /*スタイル設定*/
			CW_USEDEFAULT, /*x座標*/
			CW_USEDEFAULT, /*y座標*/
			width, /*横幅*/
			height, /*縦幅*/
			NULL, /*親ウインドウ*/
			NULL, /*メニューバー*/
			hinstance, /*インスタンス*/
			this /*WM_CRTEATEのLPARAMに自身のポインタを送る*/);

		ret = hwnd != NULL;

		ret = ShowWindow(hwnd, nCmdShow);
		ret = UpdateWindow(hwnd);

		return ret;
	}

	/*メッセージループ（イベントドリブン用）*/
	void MessageLoop() {
		MSG msg;
		HRESULT hr;

		while ((hr = GetMessage(&msg, NULL, 0, 0)) != 0) {
			if (hr == -1) {
				post_finish();
				break;
			}
			TranslateMessage(&msg); /*メッセージの変換*/
			DispatchMessage(&msg); /*メッセージの処理*/
		}
	}

	/*メインループ（毎フレーム処理用．単一ウインドウに限る）*/
	void MainLoop() {
		MSG msg;
		do
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
				BOOL ret = GetMessage(&msg, NULL, 0, 0);
				if (ret == WM_QUIT || ret == ERROR) {
					post_finish();
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				update();
				InvalidateRect(hwnd, NULL, false);
				UpdateWindow(hwnd);
				Sleep(1);
			}
		} while (msg.message != WM_QUIT);
	}
};

int KWWin32APIBase::unique_number = 0;

LRESULT CALLBACK KWWin32APIBase::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	if (msg == WM_CREATE)
	{ /*生成時の処理*/
	  /*CreateWindowで送ったクラスインスタンスのポインタを受け取る*/
		auto pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		auto pApp = reinterpret_cast<KWWin32APIBase*>(pcs->lpCreateParams);

		/*拡張ウィンドウメモリ内のUserデータ領域に呼び出し元のポインタを保存*/
		SetWindowLongPtr(hwnd, GWLP_USERDATA, PtrToLong(pApp));

		result = 1;
	}
	else
	{ /*メッセージ処理*/
	  /*拡張ウィンドウメモリ内のUserデータから呼び出し元のポインタを取得*/
		auto pApp = reinterpret_cast<KWWin32APIBase*>(
			static_cast<LONG_PTR>(GetWindowLongPtr(hwnd, GWLP_USERDATA)));

		switch (msg)
		{
		case WM_DISPLAYCHANGE:
		{
			/*ディスプレイ設定変更時*/
			InvalidateRect(hwnd, NULL, FALSE); /*更新リージョン設定（再描画を要求）*/
		}
		result = 0;
		break;
		case WM_PAINT:
		{
			/*描画要求時*/
			pApp->draw();
			ValidateRect(hwnd, NULL); /*更新リージョン削除*/
		}
		result = 0;
		break;
		case WM_SIZE:
		{
			/*ウィンドウサイズ変更時*/
			auto width = static_cast<UINT>(LOWORD(lParam)); //幅
			auto height = static_cast<UINT>(HIWORD(lParam)); //高さ
			pApp->resize(width, height);
			InvalidateRect(hwnd, NULL, FALSE); /*更新リージョン設定（再描画を要求）*/
		}
		result = 0;
		break;
		case WM_DESTROY:
		{
			/*ウインドウ破棄時*/
			pApp->pre_finish();
			PostQuitMessage(0);/*スレッドへ終了要求を伝達*/
		}
		result = 1;
		break;
		default:
		{
			/*規定のウインドウプロシージャ呼び出し*/
			result = DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;
		}
	}
	return result;
}