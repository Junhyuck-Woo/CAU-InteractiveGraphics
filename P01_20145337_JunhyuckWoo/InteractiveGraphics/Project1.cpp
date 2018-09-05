/*
	Name: Woo Jun-Hyuck
	Student ID: 20145337
	Dead line: 2015. 9. 22 15:00
	Condition: 1. Choose mode using # keyboard
				  - "1" : Point
				  - "2" : Line
				  - "3" : Triangle
				  - "4" : Square
				  - "5" : Circle

			   2. Choose color using keyboard
			      - "r" : Red
				  - "g" : Green
				  - "b" : Blue
				  - "c" : Cyan
				  - "m" : Magenta
				  - "y" : Yellow

			   3. Quit program using 'q' key

			   4. Get vertex using mouse
				  - left button
	Additional point
		=> Using pull-down menu
		=> Right button
*/

// Header which professor provide
#include <diging.h>

// Function proto type
void display();
void Keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void movement(int button, int state, int x, int y);
void pulldown_menu(int id);
void color_menu(int id);
void shape_menu(int id);
void mousemenu();
void shapes();
void colors();

// Global variable
int WIDTH = 600;
int HEIGHT = 600;
int size = 1;
int Menu = 0;
int sub_menu1 = 0, sub_menu2 = 0;
char Color = 0;

// Points: need to create polygons
vec3* polygons = (vec3*)malloc(sizeof(vec3));
float* Round = (float*)malloc(sizeof(float));

// Display Function
void display()
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(4);
	colors();
	for (int i = 0; i < size-1; i++)
	{
		shapes();
		if (Menu == '5')
		{
			for (float j = 0; j < 360; j += 1)
			{
				glVertex2f(*(Round+i)*cos(radians(j)) + polygons[i].x, *(Round+i)*sin(radians(j)) + polygons[i].y);
			}
			glEnd();
		}
		else
		{
			glVertex2f(polygons[i].x, polygons[i].y);
		}
	}
	if (Menu != '5')
		glEnd();
	glFlush();
	glutPostRedisplay();	
}

// Control keyboard input
void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'Q' || key == 'q')
		exit(0);
	else if ('1' <=key && key <= '5')
	{
		Menu = key;
	}
	else if (key == 'r' || key == 'R' || key == 'g' || key == 'G' || 
			 key == 'b' || key == 'B' || key == 'c' || key == 'C' || 
			 key == 'y' || key == 'Y' || key == 'm' || key == 'M')
	{
		Color = key;
	}
	else
	{}
}

// Control mouse input
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		 polygons[size-1].x = (float)x / (float)WIDTH * 2.0 - 1.0;
		 polygons[size-1].y = -(float)y / (float)HEIGHT * 2.0 + 1.0;
		 *(Round + (size - 1)) = 0.2;
		 size++;
		 polygons = (vec3*)realloc(polygons, size*sizeof(vec3));
		 Round = (float*)realloc(Round, size*sizeof(float));
	}
}

// Get mouse movement point
void movement(int x, int y)
{
	if (Menu == '5' && GLUT_UP)
	{
		float bufx = 0, bufy = 0;
		bufx = (float)x / (float)WIDTH * 2.0 - 1.0;
		bufy = -(float)y / (float)HEIGHT * 2.0 + 1.0;
		*(Round+(size-2)) = sqrt(pow(bufx - polygons[size - 2].x, 2) + pow(bufy - polygons[size - 2].y, 2));
	}
}

// Control menu
void pulldown_menu(int id)
{
	// if we click the "Quit", we get 12
	if (id == 12)
		exit(0);
}
void color_menu(int id)
{
	// If we click a color name, than Color gets a number.
	// It chooses color
	Color = (char)id;
}
void shape_menu(int id)
{
	Menu = id;
}
	
// Generate pull-down menu
void mousemenu()
{
	sub_menu1 = glutCreateMenu(shape_menu);
	glutAddMenuEntry("Point", '1');
	glutAddMenuEntry("Line", '2');
	glutAddMenuEntry("Triangle", '3');
	glutAddMenuEntry("Square", '4');
	glutAddMenuEntry("Circle", '5');
	sub_menu2 = glutCreateMenu(color_menu);
	glutAddMenuEntry("Red", 'r');
	glutAddMenuEntry("Green", 'g');
	glutAddMenuEntry("Blue", 'b');
	glutAddMenuEntry("Cyan", 'c');
	glutAddMenuEntry("Margenta", 'm');
	glutAddMenuEntry("Yellow", 'y');
	glutCreateMenu(pulldown_menu);
	glutAddSubMenu("Polygon Shape", sub_menu1);
	glutAddSubMenu("Polygon Color", sub_menu2);
	glutAddMenuEntry("Quit", 12);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Control shape
void shapes()
{
	switch (Menu)
	{
	case '1':
		glBegin(GL_POINTS);
		break;
	case '2':
		glBegin(GL_LINES);
		break;
	case '3':
		glBegin(GL_TRIANGLES);
		break;
	case '4':
		glBegin(GL_QUADS);
		break;
	case '5':
		glBegin(GL_POLYGON);
		break;
	default:;
	}
}

// Control color
void colors()
{
	switch (Color)
	{
	case 'r':
		glColor3f(255, 0, 0);
		break;
	case 'g':
		glColor3f(0, 255, 0);
		break;
	case 'b':
		glColor3f(0, 0, 255);
		break;
	case 'c':
		glColor3f(0, 255, 255);
		break;
	case 'm':
		glColor3f(255, 0, 255);
		break;
	case 'y':
		glColor3f(255, 255, 0);
		break;
	default:
		glColor3f(0, 0, 0);
	}
}

int main(int argc, char**argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200,200);
	glutCreateWindow("Project1");
	glutDisplayFunc(display);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(movement);
	mousemenu();
	glutMainLoop();
	return 0;
}