#include <gl/freeglut.h>
#include <stdlib.h>
#include <iostream>
#include <SOIL.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

GLuint texture;

int position = 0;
vector<int*> board;

string setFileName()
{
	string name;
	system("dir /b *.txt");

	cin >> name;

	if (name[name.size() - 1] == 't' && name[name.size() - 2] == 'x' && name[name.size() - 3] == 't' && name[name.size() - 4] == '.');
	else
		name += ".txt";
	return name;
}

void readFile(string name)
{
	fstream file;
	int *tab;
	file.open(name, ios::in);
	string testLine;
	if (file.good())
	{
		while (!file.eof())
		{
			getline(file, testLine);
			tab = new int[16];
			board.push_back(tab);

			for (int i = 0; i< 16; i++)
			{
				switch (testLine[i])
				{
				case'0': {board[position][i]=0;};break;
				case'1': {board[position][i]=1;};break;
				case'2': {board[position][i]=2;};break;
				case'3': {board[position][i]=3;};break;
				case'4': {board[position][i]=4;};break;
				case'5': {board[position][i]=5;};break;
				case'6': {board[position][i]=6;};break;
				case'7': {board[position][i]=7;};break;
				case'8': {board[position][i]=8;};break;
				case'9': {board[position][i]=9;};break;
				case'a': {board[position][i]=10;};break;
				case'b': {board[position][i]=11;};break;
				case'c': {board[position][i]=12;};break;
				case'd': {board[position][i]=13;};break;
				case'e': {board[position][i]=14;};break;
				case'f': {board[position][i]=15;};break;
				}
			}
			position++;
		}
	}
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 
	gluOrtho2D(-24, 1048, -24, 1048);
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
}

void display()
{
	glEnable(GL_COLOR);
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (position >= board.size())
	{
		position--;
		glutLeaveMainLoop();
	}

	int modulo, temp;
	for (int i = 0; i < 16; i++)
	{
		modulo = i % 4;
		temp = modulo * 4;
		glPushMatrix();
			glTranslatef((256 * modulo), -(256*(i/4)), 0);
			glCallList(board[position][i]);
		glPopMatrix();
	}
	
	position++;
	glutPostRedisplay();
	glutSwapBuffers();
	Sleep(2000);
}

void createTable()
{
	glColor3f(0, 0, 0);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			glNewList(4 * j + i , GL_COMPILE_AND_EXECUTE);

			if((i!=0) || (j!=0))
				glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
				glTexCoord2f(i*0.25, 1 - j*0.25);
				glVertex2f(0 , 1024 );
				glTexCoord2f(i*0.25, 1 - j*0.25 - 0.25);
				glVertex2f(0, 1024 - 256);
				glTexCoord2f(i*0.25 + 0.25, 1 - j*0.25 - 0.25);
				glVertex2f(256, 1024 - 256);
				glTexCoord2f(i*0.25 + 0.25, 1 - j*0.25);
				glVertex2f(256, 1024);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, NULL);
			glEndList();
		}
	}
}

GLuint loadTexture(string path)
{
	GLuint dis;
	dis = SOIL_load_OGL_texture(
		path.c_str(),
		4,
		0,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_INVERT_Y);
	return dis;
}

void main(int argc, char **argv)
{
	string path = "lena.png";
	readFile(setFileName());
	position = 0;
	glutInit(&argc, argv);

	glutInitWindowSize(1024, 1024);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Piêtnastka");
	init();
	texture = loadTexture(path);

	createTable();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
}