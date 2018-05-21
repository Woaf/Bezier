#pragma once

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <vector>

#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&, SDL_Window&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:
	// shaderekhez szükséges változók
	GLuint m_programID; // shaderek programja

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;
	};

	struct Vertex2
	{
		glm::vec2 pos;
		glm::vec3 col;

		Vertex2(glm::vec2 _pos, glm::vec3 _col)
		{
			this->pos = _pos;
			this->col = _col;
		}
	};

	const int divisor = 100;

	std::vector<Vertex2> control_points; // stores the points of the control points
	std::vector<Vertex2> output; // stores the points of the original bezier curve
	std::vector<Vertex2> polygon; // stores the points of the control polygon

	std::vector<Vertex2> div1;
	std::vector<Vertex2> div2;

	int poly_flag = 0;
	bool draw_polygon = false;
	bool draw_dots = false;
	bool split_curve = false;
	bool elevate = false;

	std::vector<Vertex2> first_level;
	std::vector<Vertex2> second_level;
	std::vector<Vertex2> third_level;
	std::vector<Vertex2> fourth_level;
	std::vector<Vertex2> fifth_level;

	std::vector<Vertex2> left_side;
	std::vector<Vertex2> right_side;
	std::vector<Vertex2> left_output;
	std::vector<Vertex2> right_output;

	std::vector<Vertex2> elevated_points;

private: 
	void myDraw(std::vector<Vertex2>&, bool, bool, bool);
	float Bernstein(int, int, float);
	int nChooseK(int, int);

	double crossProd(glm::vec2, glm::vec2, glm::vec2);
	void Collect();

	void BuildSplitTree();

	void SplitCurve();
	void ElevatePolinom();

};
