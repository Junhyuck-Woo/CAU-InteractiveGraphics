#include "diging.h"

//--------------------------------------------------------------------------------
//              OpenGL Version Check
//--------------------------------------------------------------------------------
void version() {
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	cout << "GL Vendor   :" << vendor << endl;
	cout << "GL Renderer :" << renderer << endl;
	cout << "GL Version(string) :" << version << endl;
	cout << "GL Version(integer):" << major << "." << minor << endl;
	cout << "GLSL Version :" << glslVersion << endl;
	cout << "//-------------------------------------------------------------" << endl;
}

//-------------------------------------------------------------------------------
//                  SHADERS
//-------------------------------------------------------------------------------

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile)
{
	FILE* fp = fopen(shaderFile, "r");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);

	buf[size] = '\0';
	fclose(fp);

	return buf;
}
GLuint compile_shaders(GLchar* vsource, GLchar* fsource)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	/*	const GLchar* vertex_shader_source[] = {
	"#version 440			\n"
	"#extension GL_ARB_explicit_attrib_location:enable\n"
	"						\n"
	"void					\n"
	"main()					\n"
	"{						\n"
	"   gl_Position = vec4(0.0, 0.0, 0.5, 1.0);\n"
	"}							\n"
	};

	const GLchar* fragment_shader_source[] =
	{
	"#version 440			\n"
	"#extension GL_ARB_explicit_attrib_location:enable\n"
	"						\n"
	"out vec4 Color;		\n"
	"						\n"
	"void					\n"
	"main()					\n"
	"{						\n"
	"	Color = vec4(0.0, 1.0, 0.0, 1.0);\n"
	"}									  \n"
	};*/

	static const GLchar* vertex_shader_source = readShaderSource(vsource);
	static const GLchar* fragment_shader_source = readShaderSource(fsource);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const GLchar**)&vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const GLchar**)&fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	/*  Error Check */
	GLint compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cerr << "Vertex shader failed to compile:" << endl;
		char* logMsg = new char[100];
		glGetShaderInfoLog(vertex_shader, 100, NULL, logMsg);
		cerr << logMsg << endl; delete[] logMsg;// exit (EXIT_FAILURE);
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cerr << "fragment shader failed to compile:" << endl;
		char* logMsg = new char[100];
		glGetShaderInfoLog(fragment_shader, 100, NULL, logMsg);
		cerr << logMsg << endl; delete[] logMsg;// exit (EXIT_FAILURE);
	}

	/* Delete dynamic variables */
	delete[] vertex_shader_source;
	delete[] fragment_shader_source;
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}