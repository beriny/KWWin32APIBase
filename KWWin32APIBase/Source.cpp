#include <Windows.h>

/*�E�C���h�E�v���V�[�W���i���b�Z�[�W�����j*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	/*���b�Z�[�W����*/
	switch (msg)
	{
	case WM_DESTROY:
		/*�E�C���h�E�j���v��*/
		PostQuitMessage(0);/*�X���b�h�֏I���v����`�B*/
		break;
	default:
		/*�K��̃E�C���h�E�v���V�[�W���Ăяo��*/
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return result;
}

/*���C���֐�*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR pCmd, int nCmd)
{
	HRESULT hr;

	/*�g���E�C���h�E�N���X�̍쐬*/
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX)); /*���g�p�̃p�����[�^��0*/
	wcex.cbSize = sizeof(WNDCLASSEX); /*�\���̃T�C�Y*/
	wcex.style = CS_HREDRAW | CS_VREDRAW; /*�E�C���h�E�X�^�C���i�c���T�C�Y�ύX�ōĕ`��j*/
	wcex.lpfnWndProc = WndProc; /*�E�C���h�E�v���V�[�W���ݒ�*/
	wcex.hInstance = hInst; /*�n���h���C���X�^���X*/
	wcex.lpszClassName = "Win32App"; /*�E�C���h�E�N���X��*/

									 /*�E�C���h�E�N���X�̓o�^*/
	if (RegisterClassEx(&wcex) == 0)
		return 0;

	/*�E�C���h�E�̍쐬*/
	HWND hwnd = CreateWindow(
		"Win32App", /*�E�C���h�E�N���X��*/
		"Win32 Application", /*�^�C�g��*/
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, /*�X�^�C���ݒ�*/
		CW_USEDEFAULT, /*x���W*/
		CW_USEDEFAULT, /*y���W*/
		CW_USEDEFAULT, /*����*/
		CW_USEDEFAULT, /*���ĕ�*/
		NULL, /*�e�E�C���h�E*/
		NULL, /*���j���[�o�[*/
		hInst, /*�C���X�^���X*/
		NULL /*WM_CRTEATE�œn��LPARAM�̒l*/);

	/*�E�C���h�E�\���ݒ�*/
	ShowWindow(hwnd, nCmd);
	UpdateWindow(hwnd);

	/*���b�Z�[�W���[�v*/
	MSG msg;
	while ((hr = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (hr == -1) break;
		TranslateMessage(&msg); /*���b�Z�[�W�̕ϊ�*/
		DispatchMessage(&msg); /*���b�Z�[�W�̏���*/
	}

	return 0;
}