#include "SDL.h"
#include <queue>
#include <Mmsystem.h>
#include <vector>
#pragma comment( lib,"winmm.lib" )
#pragma comment(lib,"yuv.lib") 
#pragma once

struct yuvFrame
{
	u_char *yuv;
};

class SDLWindow
{
public:
	SDLWindow(void);
	~SDLWindow(void);
	void createWindow(CWnd *pCwnd);
	void init(int width, int height,int fps);	
	void pushYUV(u_char *yuv);
	void destory();

private:	
	SDL_Texture * pTexture;
	SDL_Rect sdlRT;
	SDL_Rect dstRT;
	int iPitch;
	SDL_Renderer * pRender;
	SDL_RendererInfo info;
	CWnd *mPtr;

	CWnd *pCwnd; 
	int width; 
	int height;
	int fps;

	HANDLE pid_sdleventThread;  
	static DWORD CALLBACK sdleventThread(LPVOID);
	DWORD SDLWindow::handSdlEvent();

	HANDLE pid_playthread;  
	static DWORD CALLBACK playThread(LPVOID);
	DWORD SDLWindow::playYUV();

	std::queue <u_char *> yuvList;
	std::vector<u_char> yuvBuffer;
	
	CRITICAL_SECTION lock;
	DWORD lastTime;

	bool isStop;
public:
	SDL_Window * pWindow;
	SDL_Rect rect;
};

