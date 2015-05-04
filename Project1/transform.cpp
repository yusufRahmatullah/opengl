#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
using namespace std;

// new data structure
struct attributes{
	GLfloat coord3d[3];
	GLfloat v_color[3];
};
// --------------------------

// Data and global variable
// -----------------
GLfloat triangle_vertices[] = {
	0.0, 0.8,
	-0.8, -0.8,
	0.8, -0.8
};
GLuint program;
GLint attribute_coord2d;
GLint attribute_coord3d;
GLint uniform_m_transform;
GLuint vbo_triangle, vbo_triangle_colors;
GLint attribute_v_color;
// -----------------

int init_resources(void)
{
	// init color for the triangle
	GLfloat triangle_colors[] = {
		1.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 0.0
	};
	glGenBuffers(1, &vbo_triangle_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);
	
	// init vertex resource
	GLint compile_ok = GL_FALSE;
	GLint link_ok = GL_FALSE;
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const char * vs_source =
		"#version 140\n"
		"attribute vec2 coord2d;"
		"attribute vec3 v_color;"
		"varying vec3 f_color;"
		"void main(void){"
		"	gl_Position = vec4(coord2d, 0.0, 1.0);"
		"	f_color = v_color;"
		"}";
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
	if(!compile_ok)
	{
		cout << "Error in vertex shader" << endl;
		return 0;
	}

	// init fragment resource
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char * fs_source =
		"#version 140\n"
		"varying vec3 f_color;"
		"void main(void) {"
		"	gl_FragColor = vec4(f_color.x, f_color.y, f_color.z, 1.0);"
		"}";
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	if(!compile_ok)
	{
		cout << "Error in fragmnent shader" << endl;
		return 0;
	}

	// execute program
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if(!link_ok)
	{
		cout << "glLinkProgram : " << link_ok << endl;
		return 0;
	}

	// pass the triangle vertices to the vertex shader
	const char * attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord2d == -1)
	{
		cout << "Could not bind attribute " << attribute_name << endl;
		return 0;
	}

	// pass the triangle color to the vertex shader
	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if(attribute_v_color == -1)
	{
		cout << "Could not bind attirbute "<<attribute_name<<endl;
		return 0;
	}

	return 1;
}

void onDisplay()
{
	// clear the background as white
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// call program
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);
	glEnableVertexAttribArray(attribute_v_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_colors);

	glVertexAttribPointer(
		attribute_coord2d,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		triangle_vertices
	);

	glVertexAttribPointer(
		attribute_v_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	);
	
	// push each element
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// done with attribute
	glDisableVertexAttribArray(attribute_coord2d);
	glDisableVertexAttribArray(attribute_v_color);

	// display the result
	glutSwapBuffers();
}
 
void onIdle()
{
	// move every 5 second
	// rotate 45 degree every second;
	float move = sinf(glutGet(GLUT_ELAPSED_TIME) / 1000.0 * (2 * 3.14) / 5);
	float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;

	glm::vec3 axis_z(0, 0, 1);
	glm::mat4 m_transform = glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, 0.0))
		* glm::rotate(glm::mat4(1.0f), angle, axis_z);

	glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, glm::value_ptr(m_transform));

	glUseProgram(program);
	glutPostRedisplay();
}

void free_resources()
{
	glDeleteProgram(program);
}
 
int main(int argc, char* argv[])
{
  /* Glut-related initialising functions */
  glutInit(&argc, argv);
  glutInitContextVersion(3,1);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutIdleFunc(onIdle);
  glutCreateWindow("My First Triangle");
 
  /* Extension wrangler initialising */
  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK)
  {
	  cout << "Error : " << glewGetErrorString(glew_status) << endl;
	  return EXIT_FAILURE;
  }
 
  /* When all init functions run without errors,
  the program can initialise the resources */
  if (init_resources())
  {
    /* We can display it if everything goes OK */
    glutDisplayFunc(onDisplay);
    glutMainLoop();
  }
 
  /* If the program exits in the usual way,
  free resources and exit with a success */
  free_resources();
  return EXIT_SUCCESS;
}