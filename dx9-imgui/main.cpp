#include "overlay/overlay.h"







int main(int argc, char* argv[]) {
	FreeConsole();

	HWND hWnd = FindWindow(NULL, L"Counter-Strike 2");
	if (hWnd == NULL) {
		return 1;
	}
	Overlay::SetTargetWindow(hWnd);
	Overlay::Render();
	return 0;
}