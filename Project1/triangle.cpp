#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

// data structure
struct attributes {
	GLfloat coord3d[3];
	GLfloat v_color[3];
};

// Data and global variable
// -----------------
GLuint program;
GLint attribute_coord2d;
GLint attribute_coord3d;
GLint attribute_v_color;
GLuint vbo_triangle;
GLuint vbo_triangle_colors;
GLuint uniform_fade;
GLuint uniform_m_transform;
// -----------------

// for reading file and convert to array of char
char* file_read(const char* filename)
{
  FILE* input = fopen(filename, "rb");
  if(input == NULL) return NULL;
 
  if(fseek(input, 0, SEEK_END) == -1) return NULL;
  long size = ftell(input);
  if(size == -1) return NULL;
  if(fseek(input, 0, SEEK_SET) == -1) return NULL;
 
  /*if using c-compiler: dont cast malloc's return value*/
  char *content = (char*) malloc( (size_t) size +1  ); 
  if(content == NULL) return NULL;
 
  fread(content, 1, (size_t)size, input);
  if(ferror(input)) {
    free(content);
    return NULL;
  }
 
  fclose(input);
  content[size] = '\0';
  return content;
}

// for debugging shader
void print_log(GLuint object)
{
	GLint log_length = 0;
	if(glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if(glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else
	{
		cout << "printlog : Not a shader or a program "<< endl;
			return;
	}

	char* log = (char*) malloc(log_length);

	if(glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if(glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);

	cout << "log : " << log << endl;
	free(log);
}

// for compile the shader from 'filename' with error handling
GLuint create_shader(const char* filename, GLenum type)
{
	const GLchar* source = file_read(filename);
	if(source == NULL)
	{
		cout << "Error opening "<<filename<<endl;
		return 0;
	}
	GLuint res = glCreateShader(type);
	 const GLchar* sources[2] = {
		"#version 140\n",
    source };
	glShaderSource(res, 2, sources, NULL);
	free((void*)source);

	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if(compile_ok == GL_FALSE)
	{
		cout << filename << ":";
		print_log(res);
		glDeleteShader(res);
		return 0;
	}

	return res;
}

// for initiate resources tha used in program
int init_resources(void)
{

	GLint link_ok = GL_FALSE;

	/*
	// init VBO
	GLfloat triangle_vertices[] = {
		0.0, 0.8,
		-0.8, -0.8,
		0.8, -0.8
	};
	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW); 

	// init color VBO
	GLfloat triangle_colors[] = {
		1.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 0.0
	};
	glGenBuffers(1, &vbo_triangle_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);
	*/
	
	// combine triangle attribute
	GLfloat triangle_attribute[] = {
		0.0, 0.8, 1.0, 1.0, 0.0,
		-0.8, -0.8, 0.0, 0.0, 1.0,
		0.8, -0.8, 1.0, 0.0, 0.0
	};
	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_attribute), triangle_attribute, GL_STATIC_DRAW);
	
	/*
	// init 3d triangle attribute
	struct attributes tri3d_attributes[] = {
		{{0.0, 0.8, 0.0}, {1.0, 1.0, 0.0}},
		{{-0.8, -0.8, 0.0}, {0.0, 0.0, 1.0}},
		{{0.8, -0.8, 0.0}, {1.0, 0.0, 0.0}}
	};
	*/

	// init vertex resource
	// init fragment resource
	GLuint vs, fs;
	if ((vs = create_shader("triangle.vertex", GL_VERTEX_SHADER)) == 0) return 0;
	if ((fs = create_shader("triangle.fragment", GL_FRAGMENT_SHADER)) == 0) return 0;

	// execute program
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if(!link_ok)
	{
		cout << "glLinkProgram : " << link_ok << endl;
		cout << "log : ";
		print_log(program);
		return 0;
	}

	
	// pass the triangle vertices to the vertex shader
	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord2d == -1)
	{
		cout << "Could not bind attribute " << attribute_name << endl;
		return 0;
	}

	// pass the triangle colors to the vertex shader
	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if(attribute_v_color == -1)
	{
		cout << "Could not bind attribute " << attribute_name << endl;
		return 0;
	}
	

	// pass the uniform attribute
	const char* uniform_name = "fade";
	uniform_fade = glGetUniformLocation(program, uniform_name);
	if(uniform_fade == -1)
	{
		cout << "Could not bind uniform " << uniform_name << endl;
		return 0;
	}

	/*
	// pass the triangle3D attribute
	const char* attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord3d == -1)
	{
		cout << "Could not bind uniform " << uniform_name << endl;
		return 0;
	}

	// pass the unirform transform
	uniform_name = "m_transform";
	uniform_m_transform = glGetUniformLocation(program, uniform_name);
	if(uniform_m_transform == -1)
	{
		cout << "Could not bind uniform " << uniform_name << endl;
		return 0;
	}
	*/

	return 1;
}

void onDisplay()
{
	// enable alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// clear the background as white
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// call program
	glUseProgram(program);
	
	/*
	// describe VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glEnableVertexAttribArray(attribute_coord2d);
	glVertexAttribPointer(
		attribute_coord2d,		// attribute
		2,						// number of elements per vertex, here (x,y)
		GL_FLOAT,				// the type of each element
		GL_FALSE,				// take triangle_vertices as-is
		0,						// no extra data betwwen each position
		0		// pointer to the c array
	);

	// describe vbo color
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_colors);
	glEnableVertexAttribArray(attribute_v_color);
	glVertexAttribPointer(
		attribute_v_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	);
	*/

	// describe triangle attribute
	glEnableVertexAttribArray(attribute_coord2d);
	glEnableVertexAttribArray(attribute_v_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glVertexAttribPointer(
		attribute_coord2d,	
		2,					
		GL_FLOAT,			
		GL_FALSE,			
		5 * sizeof(GLfloat),
		0
	);
	glVertexAttribPointer(
		attribute_v_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		(GLvoid*) (2 * sizeof(GLfloat))
	);
	
	/*
	// describe triangle3D attribute
	glVertexAttribPointer(
		attribute_coord3d,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct attributes),
		0
	);
	*/

	// push each element in buffer vertices to the vertex shader
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// donte with the attribute
	glDisableVertexAttribArray(attribute_coord2d);
	glDisableVertexAttribArray(attribute_v_color);

	// display the result
	glutSwapBuffers();
}

void onIdle()
{
	float move = sinf(glutGet(GLUT_ELAPSED_TIME) / 1000.0 * (2*M_PI) / 5);
	float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;
	glm::vec3 axis_z(0, 0, 1);
	glm::mat4 m_transform = glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, 0.0))
		* glm::rotate(glm::mat4(1.0f), angle, axis_z);
	float cur_fade = sinf(glutGet(GLUT_ELAPSED_TIME) / 1000.0 * (2*M_PI) / 5) /2 + 0.5;
	glUseProgram(program);
	glUniform1f(uniform_fade, cur_fade);
	//glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, glm::value_ptr(m_transform));
	glutPostRedisplay();
}
 
void free_resources()
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_triangle);
}
 
int main(int argc, char* argv[])
{
  /* Glut-related initialising functions */
  glutInit(&argc, argv);
  glutInitContextVersion(3,1);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH|GLUT_ALPHA);
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