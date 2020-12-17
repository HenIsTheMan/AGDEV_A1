#pragma once
#include "../World/Scene.h"

class App final: public Singleton<App>{
	explicit App(const App&) = delete;
	explicit App(App&&) noexcept = delete;
	App& operator=(const App&) = delete;
	App& operator=(App&&) noexcept = delete;

	friend Singleton<App>;
public:
	~App();
	bool Init();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;
private:
	App();
	bool InitOptions() const;
	bool fullscreen;
	float elapsedTime;
	float lastFrameTime;
	Scene scene;
	static const GLFWvidmode* mode;
	static GLFWwindow* win;
};

template <class T>
inline T& operator++(T& myEnum){
	myEnum = T((int)myEnum + 1);
	return myEnum;
}