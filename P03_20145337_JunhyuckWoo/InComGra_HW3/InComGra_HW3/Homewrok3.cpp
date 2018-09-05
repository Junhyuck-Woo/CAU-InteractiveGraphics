#include <diging.h>

const int NumVertices = 36;
float Theta[3] = { 0.0, 0.0, 0.0 };
float degree = -0.0;
int Axis, Index = 0;;
enum { Xaxis, Yaxis, Zaxis };
vec4 points[NumVertices];
vec4 colors[NumVertices];

void quad(int a, int b, int c, int d);
void colorcube(); void init();

vec4 vertex_colors[8] = {
	vec4(0.0, 0.0, 0.0, 1.0), // black 
	vec4(1.0, 0.0, 0.0, 1.0), // red 
	vec4(1.0, 1.0, 0.0, 1.0), // yellow 
	vec4(0.0, 1.0, 0.0, 1.0), // green 
	vec4(0.0, 0.0, 1.0, 1.0), // blue 
	vec4(1.0, 0.0, 1.0, 1.0), // magenta
	vec4(1.0, 1.0, 1.0, 1.0), // white
	vec4(0.0, 1.0, 1.0, 1.0)  // cyan 
};

vec4 vertices[8] = {
	vec4(-0.3, -0.3, 0.3, 1.0),
	vec4(-0.3, 0.3, 0.3, 1.0),
	vec4(0.3, 0.3, 0.3, 1.0),
	vec4(0.3, -0.3, 0.3, 1.0),
	vec4(-0.3, -0.3, -0.3, 1.0),
	vec4(-0.3, 0.3, -0.3, 1.0),
	vec4(0.3, 0.3, -0.3, 1.0),
	vec4(0.3, -0.3, -0.3, 1.0)
};

void cube()
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < NumVertices; i++)
	{
		glColor4f(colors[i].x, colors[i].y, colors[i].z, colors[i].w);
		glVertex4f(points[i].x, points[i].y, points[i].z, points[i].w);
	}
	glEnd();
}

void colorcube() {
	quad(1, 0, 3, 2); quad(2, 3, 7, 6);
	quad(3, 0, 4, 7); quad(6, 5, 1, 2);
	quad(4, 5, 6, 7); quad(5, 4, 0, 1);
}

void quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

void recursion(int num, mat4 *m, float size)
{
	if (num == 0)
	{

		glPushMatrix();
		glLoadMatrixf(matrix_to_float(*m));
		cube();
	}
	else
	{
		glPushMatrix();
		glLoadMatrixf(matrix_to_float(*m));
		cube();
		*m *= Translate(size, 0.0, 0.0);
		*m *= RotateX(Theta[Xaxis])*RotateY(Theta[Yaxis])*RotateZ(Theta[Zaxis]);
		*m *= Scale(1.0 / 3);
		recursion(--num, m, size);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mat4 m(1.0);
	m *= LookAt(0, 0.5*sin(radians(degree)), 0.5*cos(radians(degree)), 0, 0, 0, 0, 1 * cos(radians(degree)), 0);
	m *= RotateX(Theta[Xaxis])*RotateY(Theta[Yaxis])*RotateZ(Theta[Zaxis]);
	recursion(2, &m, 0.6);

	glutSwapBuffers();
}

void glutgraphicinit()
{
	colorcube();
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);
}

void idle()
{
	//if (degree<180)
	degree += 0.05;
	Theta[Axis] += 0.05;
	if (Theta[Axis] > 360.0)
		Theta[Axis] -= 360.0;
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON: Axis = Xaxis;
			break;
		case GLUT_MIDDLE_BUTTON: Axis = Yaxis;
			break;
		case GLUT_RIGHT_BUTTON: Axis = Zaxis;
			break;
		}
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Homework 3");
	glutgraphicinit();
	glutMouseFunc(mouse);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}