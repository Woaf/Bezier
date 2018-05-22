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

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.2f, 0.675f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	//
	// geometria letrehozasa
	//

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	//glBufferData( GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
	//			  sizeof(vert),		// ennyi bájt nagyságban
	//			  vert,	// errõl a rendszermemóriabeli címrõl olvasva
	//			  GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat
	

	// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		(GLuint)0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		2,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex2),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	); 

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		(GLuint)1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex2),
		(void*)(sizeof(glm::vec2)) );

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	glEnable(GL_PROGRAM_POINT_SIZE);
	glLineWidth(3);

	//
	// shaderek betöltése
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// attributomok osszerendelese a VAO es shader kozt
	glBindAttribLocation( m_programID, 0, "vs_in_pos");
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
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

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

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

void CMyApp::myDraw(std::vector<Vertex2>& param, bool _point, bool linestrip, bool trinagle)
{
	// hozzunk létre egy új VBO erõforrás nevet
	glBindVertexArray(m_vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		param.size() * sizeof(Vertex2),		// ennyi bájt nagyságban
		&param[0],	// errõl a rendszermemóriabeli címrõl olvasva
		GL_STATIC_DRAW);

	// shader bekapcsolasa
	glUseProgram(m_programID);

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	// kirajzolás

	if(_point)
		glDrawArrays(GL_POINTS, 0, param.size());
	if(linestrip)
		glDrawArrays(GL_LINE_STRIP, 0, param.size());
	if(trinagle)
		glDrawArrays(GL_TRIANGLE_FAN, 0, control_points.size());

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);
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
	polygon.emplace_back(Vertex2(start, glm::vec3(0,0,0)));
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
			else if (abs(val) < 0.0005)
			{
				next = control_points[i].pos;
			}
		}

		if(next.x == start.x && next.y == start.y)
			break;

		polygon.emplace_back(Vertex2(current, glm::vec3(0,0,0)));
		current = next;
	}
}

void CMyApp::BuildSplitTree()
{
	pyramid.push_back(control_points);

	for (int i = control_points.size() - 1; i >= 1; i--)
	{
		int h = control_points.size() - i - 1;
		std::vector<Vertex2> temp;
		for (int j = 0; j < i; j++)
		{
			temp.emplace_back(
				Vertex2(
					glm::vec2((pyramid[h][j].pos + pyramid[h][j + 1].pos) / 2.0f),
					glm::vec3(1, 0, 1)
				)
			);
		}
		pyramid.push_back(temp);
	}
}

void CMyApp::SplitCurve()
{
	
		BuildSplitTree();

		left_side.emplace_back(pyramid[0][0]);
		for (int i = 0; i < pyramid.size(); i++)
		{
			left_side.emplace_back(pyramid[i][0]);
		}

		for (int i = pyramid.size()-1; i >= 0; i--)
		{
			right_side.emplace_back(pyramid[i][pyramid[i].size()-1]);
		}
		right_side.emplace_back(control_points.at(control_points.size() - 1));


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
		/*
		*/

}

void CMyApp::ElevatePolinom()
{
	float size = control_points.size()+1;
	elevated_points.clear();
	elevated_points.emplace_back(control_points.at(0));

	for (int i = 1; i < size-1; i++)
	{
		elevated_points.emplace_back(
			Vertex2(
				glm::vec2( ((float)i / (float)size))  * control_points.at(i-1).pos 
					+ (1 - ((float)i / (float)size)) * control_points.at(i).pos,
				glm::vec3(0.4, 0.7, 0.5))
		);
	}
	elevated_points.emplace_back(control_points.at(size-2));
}

void CMyApp::ReducePolinom()
{
	float n = control_points.size()-1;

	reduced_points_forward.push_back(control_points.at(0));
	for (int i = 1; i < n-1; i++)
	{
		reduced_points_forward.push_back(Vertex2(
			glm::vec2((n / (n-i)) * control_points.at(i).pos
				- ((float)i / (n-i)) * reduced_points_forward.at(i-1).pos),
			glm::vec3(0, 0, 0)
		));
	}
	reduced_points_forward.push_back(control_points.at(n));

	reduced_points_barckward.push_back(control_points.at(n));
	for (int i = n; i > 1; i--)
	{
		reduced_points_barckward.push_back(Vertex2(
			glm::vec2((n / (float)i) * control_points.at(i).pos
				- ((n-i) / (float)i) * reduced_points_barckward.at(n-i).pos),
			glm::vec3(0, 0, 0)
		));
	}
	reduced_points_barckward.push_back(control_points.at(0));

	/*
	*/

	std::reverse(reduced_points_barckward.begin(), reduced_points_barckward.end());


	int s_ize = reduced_points_forward.size();
	for (int i = 0; i < s_ize; i++)
	{
		std::cout << reduced_points_forward[i].pos.x << ", " << reduced_points_barckward[i].pos.x << std::endl;
	}

	reduced_points_combined.push_back(control_points[0]);
	for (int i = 1; i < s_ize-1; i++)
	{
		reduced_points_combined.push_back(Vertex2(
			(reduced_points_forward[i].pos + reduced_points_barckward[i].pos) / 2.0f,
			glm::vec3(0, 0, 0)));
	}
	reduced_points_combined.push_back(control_points[s_ize]);

	reduced_bezier.clear();
	for (int h = 1; h <= divisor; h++)
	{
		Vertex2 temp(glm::vec2(0), glm::vec3(1, 1, 0));
		for (int p = 0; p < reduced_points_combined.size(); p++)
		{
			temp.pos += glm::vec2(
				Bernstein(reduced_points_combined.size()-1, p, (float)h / (float)divisor) * reduced_points_combined.at(p).pos);
		}
		reduced_bezier.emplace_back(temp);
	}

}



void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (control_points.size() > 0)
	{
		// BEZIER
		myDraw(control_points, true, true, false);
		
		//OUTPUT KIRAJZOLASA
		myDraw(output, draw_dots, true, false);

		// CONTROL POLYGON KIRAJZOLASA
		if (draw_polygon)
		{	
			myDraw(polygon, true, true, false);
		}

		// SPLIT KIRAJZOLASA
		if (split_curve)
		{
			myDraw(left_side, true, true, false);
			myDraw(right_side, true, true, false);
			myDraw(left_output, true, true, false);
			myDraw(right_output, true, true, false);
		}
		
		if (elevate)
		{
			myDraw(elevated_points, true, true, false);
		}

		if (reduce)
		{
			myDraw(reduced_points_forward, true, true, false);
			myDraw(reduced_points_barckward, true, true, false);
			myDraw(reduced_points_combined, true, true, false);
			myDraw(reduced_bezier, false, true, false);
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
			polygon.clear();
		}
		else
		{
			draw_polygon = true;
			Collect();
		}
	}

	if (key.keysym.sym == SDLK_RIGHT)
	{
		if (draw_dots)
			draw_dots = false;
		else
			draw_dots = true;
	}

	if (key.keysym.sym == SDLK_LEFT)
	{
		if (split_curve)
		{
			split_curve = false;
			left_output.clear();
			left_side.clear();
			right_output.clear();
			right_side.clear();

			pyramid.clear();
		}
		else
		{
			split_curve = true;
			SplitCurve();
		}
		
	}

	if (key.keysym.sym == SDLK_UP)
	{
		if (elevate)
		{
			elevate = false;
		}
		else
		{
			elevated_points.clear();
			elevate = true;
			ElevatePolinom();

		}
	}

	if (key.keysym.sym == SDLK_DOWN)
	{
		if (reduce)
		{
			reduce = false;
		}
		else
		{
			reduced_points_forward.clear();
			reduced_points_barckward.clear();
			reduced_points_combined.clear();
			reduce = true;
			ReducePolinom();
		}
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

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );
}