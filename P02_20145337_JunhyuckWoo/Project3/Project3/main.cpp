/*
	Duedate: 2015. 11. 11 / 23:59 
			 minus 2015. 11. 17 / 23:59
	Writer: Woo Junyuck
	Functions
	== Key board ==
	 #1: move Front, back
	 #2: rotate body
	 #3: rotate turret
	 #4: rotate gun
	 #5: change gun
	 #6: translate_X cam
	 #7: translate_Y cam
	 #8: rotate_X cam
	 #9: rotate_Y cam
	 #0: scale cam

	 S,s: save key animation
	 P,p: play animtaion

	 B,b: base cam
	 N,n: angle1

	== Mouse ==
	Left: rotate
	Right: scale
	Center: translate
*/
#include "diging.h"
#include "cube.h"
#include "plane.h"
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
void render();
void idle();
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void Bezier_interpolate();
void shutdown();
void initialize();

//--------------------------------------------------------------
//						 Extern Function
//--------------------------------------------------------------
extern void version();
extern GLuint compile_shaders(GLchar* vsource, GLchar* fsource);
//-------------------------------------------------------------


//--------------------------------------------------------------
//						 Global variable
//--------------------------------------------------------------
// Coordinate
float base_X = 0.0, base_Y = 5.0, base_Z = 10.0;

//== Modeling
Cube *cube;
Plane *plane;
float centerX = 0, centerZ = 0;

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
float trans_x = 0, trans_y = 0, scale = 0, view = 50;
float rot_x = 0, rot_y = 0, rot_z = 0, buf_x = 0, buf_y = 0, buf_z = 0;
float mouseX, mouseY, pmouseX, pmouseY;
float scale_s = 0;
mat4 m;
mat4 projection;
vector <mat4> mvs;

GLuint rendering_program;
GLint color_location;
GLint proj_location;
GLint m_location;
//--------------------------------------------------------------

//--------------------------------------------------------------
// initialize
void initialize()
{
	// Ini
	glEnable(GL_DEPTH_TEST); // Check depth
	glDepthFunc(GL_LEQUAL);  // Setting: less than or

	rendering_program = compile_shaders("v.glsl", "f.glsl");
	glUseProgram(rendering_program);

	// modeling
	cube = new Cube();
	plane = new Plane(10.0f, 10.0f, 1, 1);
	color_location = glGetUniformLocation(rendering_program, "color");

	// Viewing
	// Camera Location (x, y, z)     || View Point  (x, y, z)  || Up vector
	m = LookAt(vec3(0.0f, 5.0f, 8.f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	float cameraZ = HEIGHT*0.5 / tan(radians(0.5*50.0));
	projection = Perspective(50, WIDTH / HEIGHT, 0.001*cameraZ, 10.0*cameraZ);

	m_location = glGetUniformLocation(rendering_program, "m_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);
	glUseProgram(rendering_program);

	mvs.push_back(m);

	// Initialize 
	for (int i = 0; i<100; i++) {
		for (int j = 0; j < NumOptions; j++){
			ani_key[j][i] = 0.0;
			option[j] = 0.0;
		}
	}
	sellect = 0; frame = 0;
}
//--------------------------------------------------------------

//--------------------------------------------------------------
// Display Function
void render()
{
	/*
		Contents
		1. Body
			Wheel
			Body
		2. Head
		3. Cannon
		4. Power Cannon
	*/
	// Base Setting
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(rendering_program);

	m = mvs.back();

	m = LookAt(//vec3(0.0f, 5.0f, 8.f),
		vec3(base_X - cam_cen_x - trans_x - option[cam_trans_x], base_Y - cam_cen_y - trans_y - option[cam_trans_y], base_Z),
		vec3(0.0f - cam_cen_x - trans_x - option[cam_trans_x], 0.75f - cam_cen_y - trans_y - option[cam_trans_y], 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
		);
	projection = Perspective(view - scale - option[cam_scale], WIDTH / HEIGHT, 0.001*(HEIGHT*0.5 / tan(radians(0.5*50.0))), 10.0*(HEIGHT*0.5 / tan(radians(0.5*50.0))));
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	m = m*Rotate(-rot_y - buf_y - option[cam_rot_y], vec3(1, 0, 0));
	m = m*Rotate(rot_x + buf_x + option[cam_rot_x], vec3(0, 1, 0));
	// Mouse Translation

	// Plane
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.3, 0.3, 0.3);
	plane->render();

	// hierarchical transformation 
	// Move front, back || Rotate base Y
	m = m*Translate(option[base_trans_x] * cos(radians(-option[base_rot_y])), 0.0, option[base_trans_x] * -sin(radians(option[base_rot_y])))
		*Rotate(option[base_rot_y], vec3(0, 1, 0))
		*Translate(option[base_trans_x] * cos(radians(-option[base_rot_y])), 0.0, option[base_trans_x] * sin(radians(option[base_rot_y])));

	// main Body
	// primitive1
	mvs.push_back(m);
	m = m*Translate(0.0, 0.4, 0.0)*Scale(.5, .5, 1.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.15, 0.15, 0.15);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	// primitive2
	mvs.push_back(m);
	m = m*Translate(0.0, 0.4, 0.0)*Scale(2.0, .5, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.15, 0.15, 0.15);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	// primitive3
	mvs.push_back(m);
	m = m*Translate(0.0, 0.7, 0.0)*Scale(1.85, .1, 1.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.15, 0.15, 0.15);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	mvs.push_back(m);
	m = m*Translate(0.0, 0.75, 0.0)*Scale(1.5, .1, 1.2);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();


	/* Wheel (14)
	   Rigth: front(2) / middle(3) / back(2)
	   Left : front(2) / middle(3) / back(2) */
	// primitive4: front Left
	mvs.push_back(m);
	m = m*Translate(1.0, 0.6, -0.5)*Scale(0.75, 0.1, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	mvs.push_back(m);
	m = m*Translate(1.0, 0.3, -0.5)*Scale(0.75, 0.5, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive5: front Left
	mvs.push_back(m);
	m = m*Translate(1.4, 0.3, -0.5)*RotateZ(45)*Scale(0.35, 0.35, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive6: front Right
	mvs.push_back(m);
	m = m*Translate(1.0, 0.6, 0.5)*Scale(0.75, 0.1, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive7: front Right
	mvs.push_back(m);
	m = m*Translate(1.0, 0.3, 0.5)*Scale(0.75, 0.5, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive8: front Right
	mvs.push_back(m);
	m = m*Translate(1.4, 0.3, 0.5)*RotateZ(45)*Scale(0.35, 0.35, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive9: back Left
	mvs.push_back(m);
	m = m*Translate(-1.0, 0.6, -0.5)*Scale(.75, .1, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive10: back Left
	mvs.push_back(m);
	m = m*Translate(-1.0, 0.3, -0.5)*Scale(.75, .5, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive11: back Left
	mvs.push_back(m);
	m = m*Translate(-1.4, 0.3, -0.5)*RotateZ(45)*Scale(0.35, 0.35, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive12: back Right
	mvs.push_back(m);
	m = m*Translate(-1.0, 0.6, 0.5)*Scale(.75, .1, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive13: back Right
	mvs.push_back(m);
	m = m*Translate(-1.0, 0.3, 0.5)*Scale(.75, .5, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive14: back Right
	mvs.push_back(m);
	m = m*Translate(-1.4, 0.3, 0.5)*RotateZ(45)*Scale(0.35, 0.35, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive15: Middle Right
	mvs.push_back(m);
	m = m*Translate(0.0, 0.6, 1.0)*Scale(.75, .1, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive16: Middle Right
	mvs.push_back(m);
	m = m*Translate(0.0, 0.3, 1.0)*Scale(.75, .5, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive17: Middle Right
	mvs.push_back(m);
	m = m*Translate(-0.4, 0.3, 1.0)*RotateZ(45)*Scale(0.35, 0.35, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive18: Middle Right
	mvs.push_back(m);
	m = m*Translate(0.4, 0.3, 1.0)*RotateZ(45)*Scale(0.35, 0.35, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive19: Middle Left
	mvs.push_back(m);
	m = m*Translate(0.0, 0.6, -1.0)*Scale(.75, .1, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive20: Middle Left
	mvs.push_back(m);
	m = m*Translate(0.0, 0.3, -1.0)*Scale(.75, .5, .5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	//primitive21: Middle Left
	mvs.push_back(m);
	m = m*Translate(-0.4, 0.3, -1.0)*RotateZ(45)*Scale(0.35, 0.35, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();
	// primitive22: Middle Left
	mvs.push_back(m);
	m = m*Translate(0.4, 0.3, -1.0)*RotateZ(45)*Scale(0.35, 0.35, 0.5);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();

	// hierarchical transformation 
	m = m*RotateY(option[head_rot_y]);

	// Head
	// primitive23
	mvs.push_back(m);
	m = m*Translate(0.0, 1.0, 0.0)*Scale(1.0, .5, 1.0);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.15, 0.15, 0.15);
	cube->render();
	m = mvs.back(); mvs.pop_back();

	mvs.push_back(m);
	m = m*Translate(0.35, 1.1, 0.0)*Scale(0.5, .4, 0.75);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.2, 0.2);
	cube->render();
	m = mvs.back(); mvs.pop_back();

	// hierarchical transformation 
	m = m*RotateZ(option[canon_rot_z])*Translate(option[canon_rot_z] / 50, -option[canon_rot_z] / 100, 0.0);

	// Canon
	// primitive24
	mvs.push_back(m);
	m = m*Translate(0.8, 1.0, 0.0)*Scale(1.0, .15, .6);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 1.0, 0.0, 0.0);
	cube->render();
	m = mvs.back(); mvs.pop_back();

	// hierarchical transformation 
	m = m*Translate(0.0, 0.0, option[blust_trans_z]);

	// primitive25
	mvs.push_back(m);
	m = m*Translate(1.2, 1.0, 0.1)*Scale(1.75, .25, .25);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.15, 0.15, 0.15);
	cube->render();
	m = mvs.back(); mvs.pop_back();

	// hierarchical transformation 
	m = m*Translate(0.0, 0.0, -2*option[blust_trans_z]);

	// primitive26
	mvs.push_back(m);
	m = m*Translate(1.2, 1.0, -0.1)*Scale(1.75, .25, .25);
	glUniformMatrix4fv(m_location, 1, GL_TRUE, m);
	glUniform3f(color_location, 0.15, 0.15, 0.15);
	cube->render();
	m = mvs.back(); mvs.pop_back();

	glutSwapBuffers();
}
//--------------------------------------------------------------

//--------------------------------------------------------------
// Idle
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

//--------------------------------------------------------------
// Inputs - Mouse, Keyboard
void move(int x, int y)
{
	pmouseX = (float)x / (float)WIDTH*2.0 - 1.0;
	pmouseY = -(float)y / (float)HEIGHT*2.0 + 1.0;
	if (pmouseX >= 1)
		pmouseX = 1;
	else if (pmouseX <= -1)
		pmouseX = -1;
	if (pmouseY >= 1)
		pmouseY = 1;
	else if (pmouseY <= -1)
		pmouseY = -1;
	if (trans)
	{
		trans_x = 5 * (pmouseX - mouseX);
		trans_y = 5 * (pmouseY - mouseY);
	}
	if (s)
	{
		scale = 10 * (pmouseX - mouseX);
	}
	if (rotat)
	{
		buf_x = 100*(pmouseX - mouseX);
		buf_x = 10 * radians(buf_x);
		buf_y = 100*(pmouseY - mouseY);
		buf_y = 10 * radians(buf_y);
	}
}

void mouse(int button, int state, int x, int y)
{
	mouseX = (float)x / (float)WIDTH*2.0 - 1.0;
	mouseY = -(float)y / (float)HEIGHT*2.0 + 1.0;

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{	trans = true;	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
	{
		cam_cen_x += trans_x; cam_cen_y += trans_y;	
		trans = false;
	}

	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{	rotat = true; 	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		rot_x += buf_x;
		rot_y += buf_y;
		rotat = false;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{	s = true;   }
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{	
		view -= scale;
		s = false;	
	}
	trans_x = 0; trans_y = 0;
	buf_x = 0, buf_y = 0;
	mouseX = 0, mouseY = 0, pmouseX = 0, pmouseY = 0;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	int num = 0;
	float add, minus;
	switch ( key )
	{
	case '1': sellect = 0;	break; // Translate body
	case '2': sellect = 1;	break; // Rotate body
	case '3': sellect = 2;	break; // Rotate head
	case '4': sellect = 3;  break; // Rotate cannon
	case '5': sellect = 4;  break; // Chang cannon
	case '6': sellect = 5; 	break; // Trans X 
	case '7': sellect = 6;	break; // Trans Y 
	case '8': sellect = 7;  break; // Rotate X
	case '9': sellect = 8;  break; // Rotate Y
	case '0': sellect = 9;  break; // Scale
	case 'b': case 'B':// Original
		base_X = 0.0;
		base_Y = 5.0;
		base_Z = 10.0;
		break;
	case 'n':case 'N':
		base_X = 5.0;
		base_Y = 0.5;
		break;
	case '+': add = (sellect == 0 || sellect == 4) ? 0.01 : 1.0;
		option[sellect] += add;
		if (option[3] >= 30)
			option[3] = 30;
		if (option[4] >= 0.3)
			option[4] = 0.3;
		break;
	case '-': minus = (sellect == 0 || sellect == 4) ? 0.01 : 1.0;
		option[sellect] -= minus;
		if (option[4] <= 0)
			option[4] = 0;			
		if (option[3] <= 0)
			option[3] = 0;
		break;
	case 's': case 'S':
		cout << "Chodse frame number to save animarion.(0~99) " << endl << "> "; cin >> num; 
		if ((num < 0) || (num >100))
		{
			cout << "Wrong frame number. " << endl;
			break;
		}
		else
		{
			cout << "Saving is complete. " << endl << "#Frame: " << num << endl<< endl;
			controller.push_back(num);
			for (int i = 0; i < NumOptions; i++)
				ani_key[i][num] = option[i];
			break;
		}
	case 'p': case 'P':
		animate = TRUE; frame = 0; Bclock = clock();
		Bezier_interpolate();
		break;
	case 'q': case 'Q': case 033:
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}
//--------------------------------------------------------------

//--------------------------------------------------------------
// Interpolate
void Bezier_interpolate()
{
	/*
		Bezier need 3 poits to implement,
		however I just have 2 poits.
		It means I don't have a controller,
		so I have to find a control point.

		My idea is that set control as second point which i know.
		because curve don't need to higher than third point.
		I set controller as second point, than curvature will be lower than different curve.
		but I think it could be good curve.
	*/

	int prev, next;
	int i, j, t;
	float ratio;
	float a1, a2, a3, b1, b2, c;
	float t0, t1, t2, t3;

	sort(controller.begin(), controller.end());
	prev = 0;
	for (i = 0; i<controller.size(); i++) {
		next = controller.at(i);
		for (t = prev; t < next; t++) {
			for (j = 0; j<NumOptions; j++){
				ratio = ((float)(t - prev)) / ((float)(next - prev));
				ani_key[j][t] = pow((1.0 - ratio), 2) * ani_key[j][prev] + pow(ratio, 2) * ani_key[j][next] + ratio*(1 - ratio)* ani_key[j][next];
			}
		}
		prev = next;
	}

	next = 99;
	for (t = prev; t < next; t++) {
		for (j = 0; j<NumOptions; j++){
			ratio = ((float)(t - prev)) / ((float)(next - prev));
			ani_key[j][t] = pow((1.0 - ratio), 2) * ani_key[j][prev] + pow(ratio, 2) * ani_key[j][next] + ratio*(1 - ratio)* ani_key[j][next];
		}
	}
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
int main(int argc, char **argv)
{
	// Base setting
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Tank");
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