// winmain.h

BOOL InitApplication( HINSTANCE hInstance);
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK WndProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam );
void OnAccept();
void PutLogList(char * cMsg);
void OnKeyUp(WPARAM wParam, LPARAM lParam);
void UpdateScreen();
int  EventLoop();
void OnPaint();
void OnDestroy();
void Initialize();
void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD_PTR dwUSer, DWORD_PTR dw1, DWORD_PTR dw2);
MMRESULT _StartTimer(DWORD dwTime);
void _StopTimer(MMRESULT timerid);


									 