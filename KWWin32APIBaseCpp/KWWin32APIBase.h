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

		PrepareWindowClass();
		EnableWindow(SW_SHOWNORMAL);

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

		PrepareWindowClass();
		EnableWindow(nCmd);

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

		PrepareWindowClass();
		EnableWindow(SW_SHOWNORMAL);
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

		PrepareWindowClass();
		EnableWindow(nCmd);
	}

	virtual ~KWWin32APIBase() {}

	void Run(int loop_type) {
		if (loop_type == 0)
		{
			MessageLoop();
		}
		else
		{
			MainLoop();
		}
	}

protected:
	UINT width;
	UINT height;
	HINSTANCE hinstance;
	HWND hwnd;

	virtual void begin() {}
	virtual void draw() {}
	virtual void resize(int W, int H)
	{
		width = W;
		height = H;
	}
	virtual void end() {}

private:
	static int unique_number;
	std::string wndclass_name;
	std::string title_name;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	/*�E�C���h�E�N���X�̍쐬�Ɠo�^*/
	ATOM PrepareWindowClass() {
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(WNDCLASSEX)); /*���g�p�̃p�����[�^��0*/
		wcex.cbSize = sizeof(WNDCLASSEX); /*�\���̃T�C�Y*/
		wcex.style = CS_HREDRAW | CS_VREDRAW; /*�E�C���h�E�X�^�C���i�c���T�C�Y�ύX�ōĕ`��j*/
		wcex.lpfnWndProc = WndProc; /*�E�C���h�E�v���V�[�W���ݒ�*/
		wcex.hInstance = hinstance; /*�n���h���C���X�^���X*/
		wcex.lpszClassName = wndclass_name.c_str(); /*�E�C���h�E�N���X��*/

		return RegisterClassEx(&wcex);
	}

	/*�E�C���h�E�̍쐬�ƕ\���ݒ�*/
	BOOL EnableWindow(int nCmdShow) {
		BOOL ret = TRUE;

		hwnd = CreateWindow(
			wndclass_name.c_str(), /*�E�C���h�E�N���X��*/
			title_name.c_str(), /*�^�C�g��*/
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, /*�X�^�C���ݒ�*/
			CW_USEDEFAULT, /*x���W*/
			CW_USEDEFAULT, /*y���W*/
			width, /*����*/
			height, /*�c��*/
			NULL, /*�e�E�C���h�E*/
			NULL, /*���j���[�o�[*/
			hinstance, /*�C���X�^���X*/
			this /*WM_CRTEATE��LPARAM�Ɏ��g�̃|�C���^�𑗂�*/);

		ret = hwnd != NULL;

		ret = ShowWindow(hwnd, nCmdShow);
		ret = UpdateWindow(hwnd);

		return ret;
	}

	/*���b�Z�[�W���[�v�i�C�x���g�h���u���p�j*/
	void MessageLoop() {
		MSG msg;
		HRESULT hr;

		while ((hr = GetMessage(&msg, NULL, 0, 0)) != 0) {
			if (hr == -1) break;
			TranslateMessage(&msg); /*���b�Z�[�W�̕ϊ�*/
			DispatchMessage(&msg); /*���b�Z�[�W�̏���*/
		}
	}

	/*���C�����[�v�i���t���[�������p�D�P��E�C���h�E�Ɍ���j*/
	void MainLoop() {
		MSG msg;
		do
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
				BOOL ret = GetMessage(&msg, NULL, 0, 0);
				if (ret == WM_QUIT || ret == ERROR) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
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
	{ /*�������̏���*/
	  /*CreateWindow�ő������N���X�C���X�^���X�̃|�C���^���󂯎��*/
		auto pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		auto pApp = reinterpret_cast<KWWin32APIBase*>(pcs->lpCreateParams);

		/*�g���E�B���h�E����������User�f�[�^�̈�ɌĂяo�����̃|�C���^��ۑ�*/
		SetWindowLongPtr(hwnd, GWLP_USERDATA, PtrToLong(pApp));

		result = 1;
	}
	else
	{ /*���b�Z�[�W����*/
	  /*�g���E�B���h�E����������User�f�[�^����Ăяo�����̃|�C���^���擾*/
		auto pApp = reinterpret_cast<KWWin32APIBase*>(
			static_cast<LONG_PTR>(GetWindowLongPtr(hwnd, GWLP_USERDATA)));

		switch (msg)
		{
		case WM_DISPLAYCHANGE:
		{
			/*�f�B�X�v���C�ݒ�ύX��*/
			InvalidateRect(hwnd, NULL, FALSE); /*�X�V���[�W�����ݒ�i�ĕ`���v���j*/
		}
		result = 0;
		break;
		case WM_PAINT:
		{
			/*�`��v����*/
			pApp->draw();
		}
		result = 0;
		break;
		case WM_SIZE:
		{
			/*�E�B���h�E�T�C�Y�ύX��*/
			auto width = static_cast<UINT>(LOWORD(lParam)); //��
			auto height = static_cast<UINT>(HIWORD(lParam)); //����
			pApp->resize(width, height);
			ValidateRect(hwnd, NULL); /*�X�V���[�W�����폜*/
		}
		result = 0;
		break;
		case WM_DESTROY:
		{
			/*�E�C���h�E�j����*/
			pApp->end();
			PostQuitMessage(0);/*�X���b�h�֏I���v����`�B*/
		}
		result = 1;
		break;
		default:
		{
			/*�K��̃E�C���h�E�v���V�[�W���Ăяo��*/
			result = DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;
		}
	}
	return result;
}