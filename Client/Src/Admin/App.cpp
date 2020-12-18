#include "App.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Vendor/stb_image.h"

extern float dt;
extern bool endLoop;
extern int optimalWinXPos;
extern int optimalWinYPos;
extern int optimalWinWidth;
extern int optimalWinHeight;
extern int winWidth;
extern int winHeight;

const GLFWvidmode* App::mode = nullptr;
GLFWwindow* App::win = nullptr;

App::App():
	fullscreen(false),
	elapsedTime(0.f),
	lastFrameTime(0.f),
	scene()
{
	if(!InitAPI(win)){
		(void)puts("Failed to init API\n");
		endLoop = true;
	}
	(void)Init();
}

App::~App(){
	glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
}

bool App::Init(){
	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	(void)InitOptions();
	(void)scene.Init();
	glPointSize(10.f);
	glLineWidth(4.0f);

	return true;
}

bool App::InitOptions() const{
	//Stencil buffer usually contains 8 bits per stencil value that amts to 256 diff stencil values per pixel
	//Use stencil buffer operations to write to the stencil buffer when rendering frags (read stencil values in the same or following frame(s) to pass or discard frags based on their stencil value)
	glEnable(GL_STENCIL_TEST); //Discard frags based on frags of other drawn objs in the scene

	glEnable(GL_DEPTH_TEST); //Done in screen space after the frag shader has run and after the stencil test //(pass ? frag is rendered and depth buffer is updated with new depth value : frag is discarded)

	glEnable(GL_BLEND); //Colour resulting from blend eqn replaces prev colour stored in the colour buffer

	glEnable(GL_CULL_FACE); //Specify vertices in CCW winding order so front faces are rendered in CW order while... //Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV

	glEnable(GL_PROGRAM_POINT_SIZE);
	//glEnable(GL_MULTISAMPLE); //Enabled by default on most OpenGL drivers //Multisample buffer (stores a given amt of sample pts per pixel) for MSAA
	//Multisampling algs are implemented in the rasterizer (combination of all algs and processes that transform vertices of primitives into frags, frags are bound by screen resolution unlike vertices so there is almost nvr a 1-on-1 mapping between... and it must decide at what screen coords will each frag of each interpolated vertex end up at) of the OpenGL drivers
	//Poor screen resolution leads to aliasing as the limited amt of screen pixels causes some pixels to not be rendered along an edge of a fig //Colour output is stored directly in the framebuffer if pixel is fully covered and blending is disabled

	//glEnable(GL_FRAMEBUFFER_SRGB); //Colours from sRGB colour space are gamma corrected after each frag shader run before they are stored in colour buffers of all framebuffers
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	return true;
}

void App::Update(){
	if(glfwWindowShouldClose(App::win)){
		endLoop = true;
		return;
	}

	float currFrameTime = (float)glfwGetTime();
	dt = currFrameTime - lastFrameTime;
	lastFrameTime = currFrameTime;

	elapsedTime += dt;
	static float toggleFullscreenBT = 0.f;
	if(Key(VK_F1) && toggleFullscreenBT <= elapsedTime){
		if(fullscreen){
			glfwSetWindowMonitor(win, 0, optimalWinXPos, optimalWinYPos, optimalWinWidth, optimalWinHeight, GLFW_DONT_CARE);
			fullscreen = false;
		} else{
			glfwSetWindowMonitor(win, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
			fullscreen = true;
		}
		toggleFullscreenBT = elapsedTime + .5f;
	}

	scene.Update(win);
}

void App::PreRender() const{
}

void App::Render(){
	glViewport(0, 0, 2048, 2048);

	glViewport(0, 0, winWidth, winHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.f, 0.82f, 0.86f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.ForwardRender();
}

void App::PostRender() const{
	glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
}