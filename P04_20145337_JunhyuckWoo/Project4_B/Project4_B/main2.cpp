#include "diging.h"
#include "plane.h"
#include "vbomesh.h"
#include "bmpreader.h"
#include <math.h>
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

#define CameraX 350
#define CameraY 200
#define CameraZ 350
#define AtX 0
#define AtY 0
#define AtZ 0
#define UpX 0
#define UpY 0.8
#define UpZ 0
//-------------------------------------------------------------

//--------------------------------------------------------------
//                         Functions
//-------------------------------------------------------------
void startup();
void render();
void idle();
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
	VBOMesh *ships[3];
	mat4 model[13];
	int loc[13][2];
public:

	VBOMesh *get(int index){
		return ships[index];
	}
	void set(string name, int index){
		ships[index] = new VBOMesh(name.c_str(), false, true, true);
	}

	mat4 locationGet(int index){
		return model[index];
	}
	void locationSet(mat4 m, int index){
		model[index] = m;
	}
	void locationReset(int value){
		if (value == 0)
		{
			for (int x = 0; x < 14; x++){
				model[x] = mat4(1.0);
			}
			loc[0][0] = 0; loc[0][1] = 0;
			loc[1][0] = 80; loc[1][1] = 0;
			loc[2][0] = -80; loc[2][1] = 0;
			loc[3][0] = 0; loc[3][1] = 80;
			loc[4][0] = 0; loc[4][1] = -80;
			loc[5][0] = 140; loc[5][1] = 0;
			loc[6][0] = 80; loc[6][1] = 80;
			loc[7][0] = 0; loc[7][1] = 140;
			loc[8][0] = -80; loc[8][1] = 80;
			loc[9][0] = -140; loc[9][1] = 0;
			loc[10][0] = -80; loc[10][1] = -80;
			loc[11][0] = 0; loc[11][1] = -140;
			loc[12][0] = 80; loc[12][1] = -80;
		};
	}
	int baseLocation(int x, int y)
	{
		return loc[x][y];
	}
}Fleet;
VBOMesh *planet;
vec2 shading(0.0);
//== Animation
bool animate;
bool start = false;
int frame;
int sellect[10] = { 0, };
int option = 0;
int ani1 = 0, ani2 = 0, ani3 = 0;
float animationX = 0, animationY = 0, animationZ = 0;

//== Transformation
mat4 model;
mat4 view;
mat4 projection;
float angle = 0;
float moving = 0;

GLuint rendering_program;
GLint mv_location;
GLint proj_location;
GLuint tex_object[11];

void generate_texture(GLubyte * data, int width, int height)
{
	int x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			GLubyte c = (((x & 0x8) == 0) ^ ((y & 0x8) == 0))*255.0;
			data[(y * width + x) * 4 + 0] = c;
			data[(y * width + x) * 4 + 1] = c;
			data[(y * width + x) * 4 + 2] = c;
			data[(y * width + x) * 4 + 3] = 1.0f;
		}
	}
}

void reset()
{
	Fleet.locationReset(0);
}

void modeling()
{
	planet = new VBOMesh("sphere.obj", false, true, true);
	Fleet.set("bigShip2.obj", 0);
	Fleet.set("midShip.obj", 1);
	Fleet.set("smallShip.obj", 2);
}
void loadTexture()
{
	// Load texture file
	glActiveTexture(GL_TEXTURE0);
	tex_object[0] = BMPReader::loadTex("data/blue3.bmp");
	glActiveTexture(GL_TEXTURE1);
	tex_object[1] = BMPReader::loadTex("data/tex2.bmp"); 
	glActiveTexture(GL_TEXTURE2);
	tex_object[2] = BMPReader::loadTex("data/cement.bmp");
	glActiveTexture(GL_TEXTURE3);
	tex_object[3] = BMPReader::loadTex("data/tex3.bmp");
	glActiveTexture(GL_TEXTURE4);
	tex_object[4] = BMPReader::loadTex("data/red1.bmp");
	glActiveTexture(GL_TEXTURE5);
	tex_object[5] = BMPReader::loadTex("data/rain.bmp");
	glActiveTexture(GL_TEXTURE6);
	tex_object[6] = BMPReader::loadTex("data/blue2.bmp");
	glActiveTexture(GL_TEXTURE7);
	tex_object[7] = BMPReader::loadTex("data/ruby.bmp");
	glActiveTexture(GL_TEXTURE8);
	tex_object[8] = BMPReader::loadTex("data/moss.bmp");
	glActiveTexture(GL_TEXTURE9);
	tex_object[9] = BMPReader::loadTex("data/bump.bmp");
	glActiveTexture(GL_TEXTURE10);
	tex_object[10] = BMPReader::loadTex("data/empty.bmp");
}

void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	/**** Shaders as variables */
	rendering_program = compile_shaders("v.glsl", "f.glsl");
	glUseProgram(rendering_program);

	// modeling
	reset();
	modeling();

	// viewing
	angle = 0.957283f;
	model = mat4(1.0);
	view = LookAt(vec3(CameraX, CameraY, CameraZ), vec3(AtX, AtY, AtZ), vec3(UpX , UpY, UpZ));
	projection = Perspective(70.0f, (float)WIDTH / HEIGHT, 0.3f, 1000.0f);
	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	loadTexture();
}

void idle(){
	angle += 0.01;
	moving += 0.01;
	if (start)
	{
		if (ani1 < 1000)
		{
			ani1++;
			animationZ-=0.1;
		}
		else if (ani2<1000)
		{
			ani2++;
			animationX += 0.4;
			animationZ+=0.6;
			animationY += 0.4;
		}
		else if (ani3<1000)
		{
			ani3++;
			animationX -= 0.4;
			animationZ-=0.5;
			animationY -= 0.4;
		}
		else
		{
			ani1 = 0;
			ani2 = 0;
			ani3 = 0;
		}
	}
	glutPostRedisplay();
}

void FLeet()
{
	//======================================================================
	//					 Fleet   V
	//======================================================================
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ka"), 1, 1, 1);
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Kd"), 1, 1, 1);
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ks"), 1, 1, 1);
	glUniform1f(glGetUniformLocation(rendering_program, "Material.Shininess"), 0.1);
	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 7);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 7);
	for (int index = 0; index < 13; index++)
	{
		int type = 0;
		if (index == 0) type = 0;
		if (index == 1) type = 1;
		if (index >= 2) type = 2;
		Fleet.locationSet(Translate( Fleet.baseLocation(index, 0), 0.0,Fleet.baseLocation(index, 1)), index);
		glUniformMatrix4fv(mv_location, 1, GL_TRUE, view * Translate(300, 150, 300-moving) * Scale(0.3, 0.3, 0.3) *Fleet.locationGet(index)* Rotate(180, vec3(0, 1, 0)));
		Fleet.get(type)->render();
		/*glUniform4fv(glGetUniformLocation(rendering_program, "Light.Position"),
			1, view * Fleet.locationGet(index) * Translate(300, 160, 300));*/
	}
}
void PlaneT()
{
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ka"), 0.9f, 0.9f, 0.9f);
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Kd"), 0.7f, 0.7f, 0.7f);
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ks"), 0.1f, 0.1f, 0.1f);
	glUniform1f(glGetUniformLocation(rendering_program, "Material.Shininess"), 80.0f);

	// 1
	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 0);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 0);
	model = mat4(1.0) * Scale(4, 4, 4) * Rotate(angle, vec3(0, 1, 0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	planet->render();

	//glUniform3f(glGetUniformLocation(rendering_program, "Material.Ka"), 0.1f, 0.1f, 0.1f);
	// 2
	shading.x = 1;
	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 1);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 9);
	model = mat4(1.0);
	model *= Rotate(-angle*1.2, vec3(0, 1, 0)) * Translate(0, 0, 100) * Rotate(angle * 10, vec3(0, 1, 0)) * Scale(0.5, 0.5, 0.5);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	planet->render();
	shading.x = 0;
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ka"), 0.9f, 0.9f, 0.9f);
	// 3
	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 2);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 8);;
	model = Rotate(angle, vec3(0, 1, 0)) * Translate(200, 0, 0) * Rotate(angle * 10, vec3(0, 1, 0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	planet->render();

	// 4
	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 3);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 3);
	model = Rotate(-angle, vec3(0, 1, 0)) * Translate(0, 0, 400) * Rotate(-angle*1.3, vec3(0, 1, 0)) * Translate(0, 0, -80) * Scale(1.5, 1.5, 1.5);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	planet->render();

	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 4);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 4);
	model = Rotate(-angle, vec3(0, 1, 0)) * Translate(0, 0, 400) * Rotate(-angle*1.3, vec3(0, 1, 0)) * Translate(0, 0, 80) * Scale(1.5, 1.5, 1.5);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	planet->render();

	// 5
	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 5);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 5);
	model = Rotate(angle*1.4, vec3(0, 1, 0)) * Translate(-700, 0, 0) * Rotate(angle * 10, vec3(0, 1, 0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	planet->render();

	// 6
	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 6);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 6);
	model = Rotate(-angle*1.6, vec3(0, 1, 0)) * Translate(0, 0, 1000) * Rotate(angle * 10, vec3(0, 1, 0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	planet->render();

	// 7
	glUniform1i(glGetUniformLocation(rendering_program, "mainTex"), 7);
	glUniform1i(glGetUniformLocation(rendering_program, "adderTex"), 7); 
	model = Rotate(angle*1.25, vec3(0, 1, 0)) * Translate(0, 0, -1350) * Rotate(angle * 10, vec3(0, 1, 0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	planet->render();
}
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(rendering_program);
	view = LookAt(vec3(CameraX + sellect[1] + sellect[4]+animationX, CameraY + sellect[2] + sellect[5] + animationY, CameraZ + sellect[3] + sellect[6] + animationZ), 
		vec3(AtX + sellect[1]+animationX, AtY + sellect[2]+animationY, AtZ + sellect[3]+animationZ), 
		vec3(UpX, UpY, UpZ));
	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);


	// Lighting
	glUniform3f(glGetUniformLocation(rendering_program, "Light.Intensity"), 1, 1, 1);
	glUniform4fv(glGetUniformLocation(rendering_program, "Light.Position"), 1, mat4(.1));

	/********Object  */
	FLeet();
	PlaneT();
	glutSwapBuffers();
}

void shutdown()
{
	glDeleteTextures(11, tex_object);
	glDeleteProgram(rendering_program);
}

//--------------------------------------------------------------
// Inputs - Mouse, Keyboard
void keyboard(unsigned char key, int x, int y)
{
	int num = 0;
	float add, minus;
	switch (key)
	{
	case '1': option = 1;
			break;
	case '2': option = 2;
		break;
	case '3': option = 3;
		break;
	case '4': option = 4;
		break;
	case '5':option = 5;
		break;
	case '6':option = 6;
			break;


	case '+':sellect[option] += 10;
		break;
	case '-': sellect[option] -= 10;
		break;
	case 's':case 'S':
		start = !start;
		break;
	case 'b':case 'B':
		sellect[1] = 0;
		sellect[2] = 0;
		sellect[3] = 0;
		sellect[4] = 0;
		sellect[5] = 0;
		sellect[6] = 0;
		break;
	}
	glutPostRedisplay();
}
//--------------------------------------------------------------
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Space");
	glewInit();

	version();
	startup();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
