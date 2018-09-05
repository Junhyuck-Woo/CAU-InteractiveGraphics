#include "diging.h"
#include "cube.h"
#include "plane.h"
#include "vbomesh.h"
#include "vbotorus.h"
#include <vector>
#include <algorithm>

//--------------------------------------------------------------
//						    Define
//--------------------------------------------------------------
#define WIDTH 512
#define HEIGHT 512
#define NumOptions 10
#define base_trans_x 0
#define base_rot_y 1
#define head_rot_y 2
#define canon_rot_z 3
#define blust_trans_z 4
#define cam_trans_x 5
#define cam_trans_y 6
#define cam_rot_x 7
#define cam_rot_y 8
#define cam_scale 9
//--------------------------------------------------------------

//--------------------------------------------------------------
//                         Functions
//--------------------------------------------------------------
void initialize();
void render();
void idle();
void move(int x, int y);
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char, int x, int y);
void shutdown();
//--------------------------------------------------------------
//						 Extern Function
//--------------------------------------------------------------
extern void version();
extern GLuint compile_shaders(GLchar* vsource, GLchar* fsource);
//-------------------------------------------------------------


//--------------------------------------------------------------
//						 Global variable
//--------------------------------------------------------------
//== Modeling
class FLEET
{
	VBOMesh *ships[20];
	mat4 model[20];
public:
	FLEET()
	{
		ships[0] = new VBOMesh("bigShip.obj", false, true, true);
	}
	VBOMesh *get(int x)
	{
		return ships[x];
	}
};
class HOME
{
	VBOMesh *home[14];
	mat4 model[20];
};
class FACTORY
{
	VBOMesh *factory[4];
};
VBOMesh *cityhall;
Plane *plane;

// Coordinate
float base_X = 0.0, base_Y = 5.0, base_Z = 10.0;

//== Animation
bool animate;
int frame;
vector <int> controller;
clock_t Bclock;
float ani_key[NumOptions][100] = { 0, };
float option[NumOptions];
int sellect;

//== Transformation
bool trans, rotat, s;
float cam_cen_x = 0, cam_cen_y = 0;
float trans_x = 0, trans_y = 0, scale = 0, base = 50;
float rot_x = 0, rot_y = 0, rot_z = 0, buf_x = 0, buf_y = 0, buf_z = 0;
float mouseX, mouseY, pmouseX, pmouseY;
float scale_s = 0;
mat4 projection;
mat4 view;
vector <mat4> mvs;

//== Render
GLuint rendering_program;
GLint color_location;
GLint proj_location;
GLint mv_location;
//--------------------------------------------------------------

void modeling()
{
	plane = new Plane(100, 100, 1, 1);
	/*cityhall = new VBOMesh("cityHall.obj", false, true, true);
	for (int x = 1; x < 6; x++)
	{
	ships[x] = new VBOMesh("midShip.obj", false, true, true);
	factory[x - 1] = new VBOMesh("factory.obj", false, true, true);
	}
	for (int x = 6; x < 20; x++)
	{
	ships[x] = new VBOMesh("smallShip.obj");
	home[x - 6] = new VBOMesh("building.obj");
	}*/
}

void initialize()
{
	// Initialize
	glEnable(GL_DEPTH_TEST); // Check depth
	glDepthFunc(GL_LEQUAL);  // Setting: less than or

	rendering_program = compile_shaders("v.glsl", "f.glsl");
	glUseProgram(rendering_program);
	modeling();
	//fleet.get(0)->render();

	view = LookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = Perspective(50, WIDTH / HEIGHT, 0.02, 100.0);
	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

	glUniformMatrix4fv(mv_location, 1, GL_TRUE, mview);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);
}

void render()
{
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(rendering_program);

	glUniform3f(color_location, 1.0, 1.0, 1.0);
	plane->render();
	glutSwapBuffers();
}

void idle()
{
	clock_t Lclock;
	Lclock = clock();

	if ((animate) && (Lclock - Bclock > 150)) {
		if (frame<99) {
			frame++;
			for (int i = 0; i<NumOptions; i++)
				option[i] = ani_key[i][frame];
		}
		else animate = FALSE;

		Bclock = Lclock;
	}
	glutPostRedisplay();
}
//--------------------------------------------------------------
// Inputs - Mouse, Keyboard
void keyboard(unsigned char, int x, int y)
{

}
//--------------------------------------------------------------

//--------------------------------------------------------------
// Complete
void shutdown()
{
	glDeleteProgram(rendering_program);
}
//--------------------------------------------------------------

// Main Function
int main(int argc, char**argv)
{
	// Base setting
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Space");
	glewInit();

	initialize();
	version();
	glutIdleFunc(idle);
	glutDisplayFunc(render);
	glutMouseFunc(mouse);
	glutMotionFunc(move);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	shutdown();
	return 0;
}