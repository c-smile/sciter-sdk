//========================================================================
// Simple GLFW example by Camilla Berglund <elmindreda@glfw.org>
// 
// Modified for Sciter integration purposes by Andrew Fedoniouk <andrew@sciter.com>
//========================================================================
//! [code]

#include "glad\glad.h"

#include "linmath.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
struct GLBitmap
{
	GLBitmap();
	~GLBitmap();

	bool Update( GLuint width, GLuint height, const void * bits );
	
	GLuint _texture;
	GLuint _width;
	GLuint _height;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//void DrawBitmap( const int width, const int height, char * bits )

///////////////////////////////////////////////////////////////////////////////////////////////////////
// viewport should be set appropriately
void GLDrawScreenQuad( GLuint texture );

