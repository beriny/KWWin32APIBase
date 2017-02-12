#include "KWWin32APIBase.h"
#include <thread>


/*ÉÅÉCÉìä÷êî(C++)*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR pCmd, int nCmd)
{
	std::thread tr1([&hInst, &nCmd] {
		KWWin32APIBase App1(hInst, "TestApp1", nCmd, 640, 480);
		App1.Run(0);
	});

	std::thread tr2([&hInst, &nCmd] {
		KWWin32APIBase App2(hInst, "TestApp2", nCmd);
		App2.Run(0);
	});

	tr1.join();
	tr2.join();

	return 0;
}