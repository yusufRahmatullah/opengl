#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
using namespace std;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	// create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShadesID = glCreateShader(GL_FRAGMENT_SHADER);

	// read the vertex shaders code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		string line = "";
		while(getline(VertexShaderStream, line))
		{
			VertexShaderCode += "\n" + line;
		}
		VertexShaderStream.close();
	}

	// read the fragment shaders codce from file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open())
	{
		string line = "";
		while(getline(FragmentShaderStream, line))
		{
			FragmentShaderCode += "\n" + line;
		}
		FragmentShaderStream.close();
	}

	GLint result = GL_FALSE;
	int infoLogLength;

	// compile vertex shader
	cout << "Compiling vertex shader : "<<vertex_file_path<<endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// check vertex shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	vector<char> VertexShaderErrorMessage(infoLogLength);
	glGetShaderInfoLog(VertexShaderID, infoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	//compile fragment shader
	cout << "COmpiling fragment shader : "<<fragment_file_path<<endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShadesID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShadesID);

	// check fragment shader
	glGetShaderiv(FragmentShadesID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(FragmentShadesID, GL_INFO_LOG_LENGTH, &infoLogLength);
	vector<char> FragmentShaderErrorMessage(infoLogLength);
	glGetShaderInfoLog(FragmentShadesID, infoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// link the program
	cout << "Linking program "<< endl;
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShadesID);
	glLinkProgram(ProgramID);

	// check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
	vector<char> ProgramErrorMessage(max(infoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, infoLogLength, NULL, & ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShadesID);

	return ProgramID;
}

int main()
{
	GLuint programID = LoadShaders("ex1.vertex", "ex1.fragment");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}