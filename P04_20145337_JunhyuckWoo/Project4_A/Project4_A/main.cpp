#include "diging.h"
#include "vboteapot.h"

//--------------------------------------------------------------
//						    Define
//--------------------------------------------------------------
#define WIDTH 512
#define HEIGHT 512

//--------------------------------------------------------------
//                         Functions
//--------------------------------------------------------------
void render();
void idle();
void setup();
void shutdown();

//--------------------------------------------------------------
//						 Extern Function
//--------------------------------------------------------------
extern void version();
extern GLuint compile_shaders(GLchar* vsource, GLchar* fsource);

//--------------------------------------------------------------
//						 Global variable
//-------------------------------------------------------------
GLuint rendering_program;
VBOTeapot *teapot;


mat4 model;
mat4 view;
mat4 projection;
float angle;
float material[20][3][3] = { 
	{ { 0.135, 0.2225, 0.1575 }, { 0.54, 0.89, 0.63 }, { 0.316228, 0.316228, 0.316228 } }, // 1
	{ { 0.2125, 0.1275, 0.054 }, { 0.714, 0.4284, 0.18144 }, { 0.393548, 0.271906, 0.166721 } }, // 7
	{ { 0.0, 0.1, 0.06 }, { 0.0, 0.50980392, 0.50980392 }, { 0.50196078, 0.50196078, 0.50196078 } }, // 13
	{ { 0.0, 0.05, 0.05 }, { 0.4, 0.5, 0.5 }, { 0.04, 0.7, 0.7 } }, // 19
	{ { 0.05375, 0.05, 0.06625 }, { 0.18275, 0.17, 0.22525 }, { 0.332741, 0.328634, 0.346435 } }, // 2
	{ { 0.25, 0.25, 0.25 }, { 0.4, 0.4, 0.4 }, { 0.774597, 0.774597, 0.774597 } }, // 8
	{ { 0.0, 0.0, 0.0 }, { 0.1, 0.35, 0.1 }, { 0.45, 0.55, 0.45 } }, // 14
	{ { 0.0, 0.05, 0.0 }, { 0.4, 0.5, 0.4 }, { 0.04, 0.7, 0.04 } }, // 20
	{ { 0.25, 0.20725, 0.20725 }, { 1, 0.829, 0.829 }, { 0.296648, 0.296648, 0.296648 } }, // 3
	{ { 0.19125, 0.0735, 0.0225 }, { 0.7038, 0.27048, 0.0828 }, { 0.256777, 0.137622, 0.086014 } }, // 9
	{ { 0.0, 0.0, 0.0 }, { 0.5, 0.0, 0.0 }, { 0.7, 0.6, 0.6 } }, // 15
	{ { 0.05, 0.0, 0.0 }, { 0.5, 0.4, 0.4 }, { 0.7, 0.04, 0.04 } }, // 21
	{ { 0.1745, 0.01175, 0.01175 }, { 0.61424, 0.04136, 0.04136 }, { 0.727811, 0.626959, 0.626959 } }, // 4
	{ { 0.24725, 0.1995, 0.0745 }, { 0.75164, 0.60648, 0.22648 }, { 0.628281, 0.555802, 0.366065 } }, // 10
	{ { 0.0, 0.0, 0.0 }, { 0.55, 0.55, 0.55 }, { 0.70, 0.70, 0.70 } }, // 16
	{ { 0.05, 0.05, 0.05 }, { 0.5, 0.5, 0.5 }, { 0.7, 0.7, 0.7 } }, // 22
	{ { 0.1, 0.18725, 0.1745 }, { 0.396, 0.74151, 0.69102 }, { 0.297254, 0.30829, 0.306678 } }, // 5
	{ { 0.19225, 0.19225, 0.19225 }, { 0.50754, 0.50754, 0.50754 }, { 0.508273, 0.508273, 0.508273 } }, // 11
	{ { 0.0, 0.0, 0.0 }, { 0.5, 0.5, 0.0 }, { 0.60, 0.60, 0.50 } }, // 17
	{ { 0.05, 0.05, 0.0 }, { 0.5, 0.5, 0.4 }, { 0.7, 0.7, 0.04 } } //23
};

float shiness[20] = {12.8, 25.6, 32, 10
					, 38.4, 76.8, 32, 10
					, 10.24, 12.8, 32, 10
					, 76.8,  51.2, 32, 10
					, 12.8, 51.2, 32, 10};
GLint mv_location;
GLint proj_location;


//--------------------------------------------------------------
// Display Function
void render()
{
	int i = 0, j = -1;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(rendering_program); 

	/*glUniform4fv(glGetUniformLocation(rendering_program, "Light.Position"),
		1, view*Translate(-20, 20, -100.0));*/
	
	glUniform4fv(glGetUniformLocation(rendering_program, "Light.Position"),
		1, view*vec4(100.0f*cos(angle), 50.0, 100.0f*sin(angle), 1.0));

	/*glUniform4fv(glGetUniformLocation(rendering_program, "Light.Position"),
		1, vec4(-5, 10, -20, 1.0));*/
	for (int n = 0; n < 20; n++, i++)
	{
		if (n % 4 == 0){
			j++, i = 0;
		}
		// Material
		glUniform3f(glGetUniformLocation(rendering_program, "Material.Ka"), material[n][0][0], material[n][0][1], material[n][0][2]);
		glUniform3f(glGetUniformLocation(rendering_program, "Material.Kd"), material[n][1][0], material[n][1][1], material[n][1][2]);
		glUniform3f(glGetUniformLocation(rendering_program, "Material.Ks"), material[n][2][0], material[n][2][1], material[n][2][2]);
		glUniform1f(glGetUniformLocation(rendering_program, "Material.Shininess"), shiness[n]);
		model = mat4(1.0f); model = Translate(-12 + 8.0*i, 12 -8.0*j, 0.0) *RotateX(-90.0f);
		glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
		teapot->render();
	}
	glutSwapBuffers();
}
//--------------------------------------------------------------


//--------------------------------------------------------------
// Idle
void idle()
{
	angle += 0.01f;
	if (angle > 2.0*M_PI) angle -= 2.0*M_PI;
	
	/*if (angle < radians(-90))
		angle += 0;
	else
		angle -= 0.01f;*/
	glutPostRedisplay();
}
//--------------------------------------------------------------

//--------------------------------------------------------------
// Setup
void setup()
{
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	/**** Shaders as variables */
	rendering_program = compile_shaders("v.glsl", "f.glsl");
	glUseProgram(rendering_program);

	// modeling Teapot
	//for (int i = 0; i < 20; i++)
		teapot = new VBOTeapot(14, mat4(1.0f));

	// Viewing
	angle = 0.957283f;
	model = mat4(1.0);
	view = LookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = Ortho(-20, 20, -23, 20, -100, 100);

	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	//Lighting
	glUniform3f(glGetUniformLocation(rendering_program, "Light.Intensity"), 1.0, 1.0, 1.0);
}
//--------------------------------------------------------------

//--------------------------------------------------------------
// Complete
void shutdown()
{
	glDeleteProgram(rendering_program);
}
//--------------------------------------------------------------

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Project 4 - A");
	glewInit();

	version();
	setup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}