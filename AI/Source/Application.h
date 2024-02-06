
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

class Scene;
class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);
	static bool IsMousePressed(unsigned short key);
	static bool IsMouseJustPressed(unsigned short key);
	static void GetCursorPos(double *xpos, double *ypos);
	static int GetWindowWidth();
	static int GetWindowHeight();

	void Iterate();

	static double DELTATIME;
	static void QueueChangeScene(Scene* scene);
	static bool sceneChanging;
	static Scene* nextScene;

private:
	Application();
	~Application();

	static bool _past[3];
	static bool _current[3];

	//Declare a window object
	StopWatch m_timer;
	Scene* m_scene;
};

#endif