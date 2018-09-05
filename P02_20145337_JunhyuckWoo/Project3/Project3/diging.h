#ifndef __DIGING_H__
#define __DIGING_H__

#include <cmath>
#include <iostream>
#include "time.h"

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "freeglut.lib")
#pragma comment (lib, "glew32.lib")

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define radians(x) x*M_PI/180.0

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include "vec.h"
#include "mat.h"

using namespace std;
using namespace Diging;

#endif