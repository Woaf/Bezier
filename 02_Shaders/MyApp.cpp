#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{

	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.2f, 0.675f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)

	//
	// geometria letrehozasa
	//

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
	// t�lts�k fel adatokkal az akt�v VBO-t
	//glBufferData( GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
	//			  sizeof(vert),		// ennyi b�jt nagys�gban
	//			  vert,	// err�l a rendszermem�riabeli c�mr�l olvasva
	//			  GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat
	

	// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		(GLuint)0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		2,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex2),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	); 

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		(GLuint)1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex2),
		(void*)(sizeof(glm::vec2)) );

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

	//
	// shaderek bet�lt�se
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket t�rol� program l�trehoz�sa
	m_programID = glCreateProgram();

	// adjuk hozz� a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// attributomok osszerendelese a VAO es shader kozt
	glBindAttribLocation( m_programID, 0, "vs_in_pos");
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illessz�k �ssze a shadereket (kimen�-bemen� v�ltoz�k �sszerendel�se stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 1)
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] S�der Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
}

int CMyApp::nChooseK(int n, int k)
{
	if (k > n)
		return 0;
	if (k == 0)
		return 1;

	int result = n;
	for (int i = 2; i <= k; i++)
	{
		result *= (n - i + 1);
		result /= i;
	}
	return result;
}

float CMyApp::Bernstein(int n, int i, float t)
{
	return nChooseK(n, i) * pow(t, i) * pow((1-t), (n-i));
}

double CMyApp::crossProd(glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
	double x1 = a.x - b.x;
	double x2 = a.x - c.x;
	double y1 = a.y - b.y;
	double y2 = a.y - c.y;
	return y2 * x1 - y1 * x2;
}

void CMyApp::Collect()
{
	polygon.clear();
	std::vector<glm::vec2> colinear;
	glm::vec2 start = control_points.at(0).pos;

	for (int i = 1; i < control_points.size(); i++)
	{
		if (control_points.at(i).pos.x < start.x)
		{
			start = control_points.at(i).pos;
		}
	}

	glm::vec2 current = start;
	polygon.emplace_back(Vertex2(start, glm::vec3(0,1,1)));
	while (true)
	{
		glm::vec2 next = control_points[0].pos;
		for (int i = 1; i < control_points.size(); i++)
		{
			if (control_points[i].pos.x == current.x
				&& control_points[i].pos.y == current.y)
				continue;

			double val = crossProd(current, next, control_points[i].pos);

			if(val < 0)
			{
				next = control_points[i].pos;
				colinear.clear();
			}
			else if (abs(val) < 0.005)
			{
				next = control_points[i].pos;
			}
		}

		if(next.x == start.x && next.y == start.y)
			break;

		polygon.emplace_back(Vertex2(current, glm::vec3(0,1,1)));
		current = next;
	}
}

void CMyApp::BuildSplitTree()
{
	for (int i = 0; i < control_points.size()-1; i++)
	{
		first_level.emplace_back(Vertex2(glm::vec2((control_points[i].pos + control_points[i + 1].pos) / 2.0f),
			glm::vec3(1, 0, 1)));
	}

	for (int i = 0; i < first_level.size()-1; i++)
	{
		second_level.emplace_back(Vertex2(glm::vec2((first_level[i].pos + first_level[i + 1].pos) / 2.0f),
			glm::vec3(1, 0, 1)));
	}

	for (int i = 0; i < second_level.size()-1; i++)
	{
		third_level.emplace_back(Vertex2(glm::vec2((second_level[i].pos + second_level[i + 1].pos) / 2.0f),
			glm::vec3(1, 0, 1)));
	}

	for (int i = 0; i < third_level.size()-1; i++)
	{
		fourth_level.emplace_back(Vertex2(glm::vec2((third_level[i].pos + third_level[i + 1].pos) / 2.0f),
			glm::vec3(1, 0, 1)));
	}

	for (int i = 0; i < fourth_level.size()-1; i++)
	{
		fifth_level.emplace_back(Vertex2(glm::vec2((fourth_level[i].pos + fourth_level[i + 1].pos) / 2.0f),
			glm::vec3(1, 0, 1)));
	}

}

void CMyApp::SplitCurve()
{
	if ((control_points.size() % 2) == 0 &&
		control_points.size() >= 4)
	{
		BuildSplitTree();

		left_side.emplace_back(control_points[0]);
		left_side.emplace_back(first_level.at(0));
		left_side.emplace_back(second_level.at(0));
		left_side.emplace_back(third_level.at(0));
		left_side.emplace_back(fourth_level.at(0));
		left_side.emplace_back(fifth_level.at(0));


		right_side.emplace_back(fifth_level.at(0));
		right_side.emplace_back(fourth_level.at(1));
		right_side.emplace_back(third_level.at(2));
		right_side.emplace_back(second_level.at(3));
		right_side.emplace_back(first_level.at(4));
		right_side.emplace_back(control_points[5]);

		left_output.clear();

		for (int h = 1; h <= divisor; h++)
		{
			Vertex2 temp(glm::vec2(0), glm::vec3(0, 1, 0));
			for (int p = 0; p < left_side.size(); p++)
			{
				temp.pos += glm::vec2(
					Bernstein(left_side.size() - 1, p, (float)h / (float)divisor) * left_side.at(p).pos);
			}
			left_output.emplace_back(temp);
		}

		right_output.clear();
		for (int h = 1; h <= divisor; h++)
		{
			Vertex2 temp(glm::vec2(0), glm::vec3(1, 1, 0));
			for (int p = 0; p < right_side.size(); p++)
			{
				temp.pos += glm::vec2(
					Bernstein(right_side.size() - 1, p, (float)h / (float)divisor) * right_side.at(p).pos);
			}
			right_output.emplace_back(temp);
		}

	}
}



void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (control_points.size() > 0)
	{
		// hozzunk l�tre egy �j VBO er�forr�s nevet
		glBindVertexArray(m_vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
		glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		control_points.size()*sizeof(Vertex2),		// ennyi b�jt nagys�gban
		&control_points[0],	// err�l a rendszermem�riabeli c�mr�l olvasva
		GL_STATIC_DRAW);

		// shader bekapcsolasa
		glUseProgram(m_programID);

		// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
		glBindVertexArray(m_vaoID);

		// kirajzol�s
		glDrawArrays(GL_POINTS, 0, control_points.size());
		glDrawArrays(GL_LINE_STRIP, 0, control_points.size());
		//glDrawArrays(GL_TRIANGLE_FAN, 0, control_points.size());
		glEnable(GL_PROGRAM_POINT_SIZE);

		// VAO kikapcsolasa
		glBindVertexArray(0);

		// shader kikapcsolasa
		glUseProgram(0);

		//OUTPUT KIRAJZOLASA

		glBindVertexArray(m_vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
		glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
			output.size() * sizeof(Vertex2),		// ennyi b�jt nagys�gban
			&output[0],	// err�l a rendszermem�riabeli c�mr�l olvasva
			GL_STATIC_DRAW);

		// shader bekapcsolasa
		glUseProgram(m_programID);

		// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
		glBindVertexArray(m_vaoID);

		// kirajzol�s
		if (draw_dots)
		{
			glDrawArrays(GL_POINTS, 0, output.size());
		}
		glDrawArrays(GL_LINE_STRIP, 0, output.size());
		//glDrawArrays(GL_TRIANGLE_FAN, 0, control_points.size());


		// VAO kikapcsolasa
		glBindVertexArray(0);

		// shader kikapcsolasa
		glUseProgram(0);




		// CONTROL POLYGON KIRAJZOLASA
		if (draw_polygon)
		{
			if (poly_flag == 0)
			{
				poly_flag = 1;
				Collect();
				std::cout << "The size of the array is: " << polygon.size() << std::endl;

				for (int i = 0; i < polygon.size(); i++)
				{
					std::cout << polygon[i].pos.x << ", " << polygon[i].pos.y << std::endl;
				}

			}
			
			glBindVertexArray(m_vaoID);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
			glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				polygon.size() * sizeof(Vertex2),		// ennyi b�jt nagys�gban
				&polygon[0],	// err�l a rendszermem�riabeli c�mr�l olvasva
				GL_STATIC_DRAW);

			// shader bekapcsolasa
			glUseProgram(m_programID);

			// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
			glBindVertexArray(m_vaoID);

			// kirajzol�s
			glDrawArrays(GL_POINTS, 0, polygon.size());
			glDrawArrays(GL_LINE_STRIP, 0, polygon.size());
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, polygon.size());


			// VAO kikapcsolasa
			glBindVertexArray(0);

			// shader kikapcsolasa
			glUseProgram(0);
		}






		// SPLIT KIRAJZOLASA
		if (split_curve)
		{

			glBindVertexArray(m_vaoID);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
			glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				left_side.size() * sizeof(Vertex2),		// ennyi b�jt nagys�gban
				&left_side[0],	// err�l a rendszermem�riabeli c�mr�l olvasva
				GL_STATIC_DRAW);

			// shader bekapcsolasa
			glUseProgram(m_programID);

			// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
			glBindVertexArray(m_vaoID);

			// kirajzol�s
			glDrawArrays(GL_POINTS, 0, left_side.size());
			glDrawArrays(GL_LINE_STRIP, 0, left_side.size());
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, polygon.size());

			// VAO kikapcsolasa
			glBindVertexArray(0);
			// shader kikapcsolasa
			glUseProgram(0);

			glBindVertexArray(m_vaoID);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
			glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				right_side.size() * sizeof(Vertex2),		// ennyi b�jt nagys�gban
				&right_side[0],	// err�l a rendszermem�riabeli c�mr�l olvasva
				GL_STATIC_DRAW);

			// shader bekapcsolasa
			glUseProgram(m_programID);

			// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
			glBindVertexArray(m_vaoID);

			// kirajzol�s
			glDrawArrays(GL_POINTS, 0, right_side.size());
			glDrawArrays(GL_LINE_STRIP, 0, right_side.size());
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, polygon.size());

			// VAO kikapcsolasa
			glBindVertexArray(0);
			// shader kikapcsolasa
			glUseProgram(0);

			glBindVertexArray(m_vaoID);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
			glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				right_output.size() * sizeof(Vertex2),		// ennyi b�jt nagys�gban
				&right_output[0],	// err�l a rendszermem�riabeli c�mr�l olvasva
				GL_STATIC_DRAW);

			// shader bekapcsolasa
			glUseProgram(m_programID);

			// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
			glBindVertexArray(m_vaoID);

			// kirajzol�s
			glDrawArrays(GL_POINTS, 0, right_output.size());
			glDrawArrays(GL_LINE_STRIP, 0, right_output.size());
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, polygon.size());

			// VAO kikapcsolasa
			glBindVertexArray(0);
			// shader kikapcsolasa
			glUseProgram(0);

			glBindVertexArray(m_vaoID);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
			glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				left_output.size() * sizeof(Vertex2),		// ennyi b�jt nagys�gban
				&left_output[0],	// err�l a rendszermem�riabeli c�mr�l olvasva
				GL_STATIC_DRAW);

			// shader bekapcsolasa
			glUseProgram(m_programID);

			// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
			glBindVertexArray(m_vaoID);

			// kirajzol�s
			glDrawArrays(GL_POINTS, 0, left_output.size());
			glDrawArrays(GL_LINE_STRIP, 0, left_output.size());
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, polygon.size());

			// VAO kikapcsolasa
			glBindVertexArray(0);
			// shader kikapcsolasa
			glUseProgram(0);

		}
		
	}
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	if (key.keysym.sym == SDLK_SPACE)
	{
		if (draw_polygon)
		{
			draw_polygon = false;
			poly_flag = 0;
		}
		else
		{
			draw_polygon = true;
		}
	}

	if (key.keysym.sym == SDLK_RIGHT)
	{
		if (draw_dots)
		{
			draw_dots = false;
		}
		else
		{
			draw_dots = true;
		}
	}

	if (key.keysym.sym == SDLK_LEFT)
	{
		split_curve = true;
		SplitCurve();
	}

	if (key.keysym.sym == SDLK_s)
	{
		std::cout << "Size of the control points vector: " << control_points.size() << std::endl;
	}

}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{

}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse, SDL_Window& win)
{
	int width, height;
	
	SDL_GetWindowSize(&win, &width, &height);
	this->control_points.emplace_back( Vertex2(glm::vec2(
										(mouse.x - (width/2.0f)) / (width/ 2.0f), 
										(mouse.y - (height / 2.0f)) / -(height / 2.0f)), 
										glm::vec3(1)));

	output.clear();
	for (int h = 1; h <= divisor; h++)
	{
		Vertex2 temp(glm::vec2(0), glm::vec3(1,0,0));
		for (int p = 0; p < control_points.size(); p++)
		{
			temp.pos += glm::vec2(
				Bernstein(control_points.size() - 1, p, (float) h / (float)divisor) * control_points.at(p).pos);
		}
		output.emplace_back(temp);
	}
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a k�t param�terbe az �j ablakm�ret sz�less�ge (_w) �s magass�ga (_h) tal�lhat�
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );
}