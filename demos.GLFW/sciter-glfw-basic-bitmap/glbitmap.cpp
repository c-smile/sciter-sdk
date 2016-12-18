//========================================================================
// Simple GLFW example by Camilla Berglund <elmindreda@glfw.org>
// 
// Modified for Sciter integration purposes by Andrew Fedoniouk <andrew@sciter.com>
//========================================================================
//! [code]

#include "glbitmap.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "linmath.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
GLBitmap::GLBitmap( )
	: _texture(0u)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
GLBitmap::~GLBitmap( )
{
	if (_texture)
	{
		// if currently bound, should be cleaned up where bound
		glDeleteTextures( 1, &_texture );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
bool GLBitmap::Update( GLuint width, GLuint height, const void * bits )
{
	if (_width != width || _height != height)
	{
		// delete texture
		if (_texture != 0u)
		{
			// if currently bound, should be cleaned up where bound
			glDeleteTextures( 1, &_texture );
			_texture = 0u;
		}

		_width = width;
		_height = height;

		glGenTextures( 1, &_texture );
		
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); //unaligned for now

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, _texture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits );
		
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		//GLenum err = glGetError( );

		glBindTexture( GL_TEXTURE_2D, 0u );

		
		
	}
	else
	{
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); //unaligned for now

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, _texture );
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, bits );
		
		//GLenum err = glGetError( );

		glBindTexture( GL_TEXTURE_2D, 0u );
	}

	//TODO: errors
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
struct TechniqueScreenQuad
{
	TechniqueScreenQuad( );
	~TechniqueScreenQuad( );

	bool Draw( const GLuint texture );

	bool	_valid;
	GLuint	_vertex_array;
	GLuint	_vertex_buffer;
	GLuint	_program;
	GLuint	_location_vpos;
	GLuint	_location_s0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
TechniqueScreenQuad::TechniqueScreenQuad( )
{
	static const struct Vertex3D
	{
		float x, y;
	} triangle_quad[3] =
	{
		{ -1.0f, -1.0f },
		{ -1.0f,  3.0f },
		{ 3.0f, -1.0f }
	};

	static const char* vs_passthrough =
		"#version 330 core\n"
		"layout(location = 0) in vec2 vpos;\n"
		"out vec2 uv;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(vpos.xy, 0.0, 1.0);\n"
		"    uv.xy = vpos.xy * 0.5 + 0.5;\n"
    "    uv.y = 1.0 - uv.y;"
		"}\n";

	static const char* ps_texture =
		"#version 330 core\n"
		"in vec2 uv;\n"
		"uniform sampler2D s0;"
		"out vec4 _color;"
		"void main()\n"
		"{\n"
		"    _color = texture(s0, uv);\n"
		"}\n";

	glGenVertexArrays( 1, &_vertex_array );
	glBindVertexArray( _vertex_array );

	glGenBuffers( 1, &_vertex_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, _vertex_buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangle_quad ), triangle_quad, GL_STATIC_DRAW );

	GLchar info[1024];
	GLsizei len;

	GLuint vertex_shader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertex_shader, 1, &vs_passthrough, NULL );
	glCompileShader( vertex_shader );
	//glGetShaderInfoLog( vertex_shader, 1024, &len, info );

	GLuint fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragment_shader, 1, &ps_texture, NULL );
	glCompileShader( fragment_shader );
	//glGetShaderInfoLog( fragment_shader, 1024, &len, info );

	_program = glCreateProgram( );
	glAttachShader( _program, vertex_shader );
	glAttachShader( _program, fragment_shader );
	glLinkProgram( _program );
	glValidateProgram( _program );
	//glGetProgramInfoLog( _program, 1024, &len, info );

	_location_vpos = 0u;// glGetUniformLocation( _program, "vpos" );
	_location_s0 = glGetUniformLocation( _program, "s0" );

	glDeleteShader( vertex_shader );
	glDeleteShader( fragment_shader );

	glEnableVertexAttribArray( _location_vpos );
	glVertexAttribPointer( _location_vpos, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 2, (void*)0 );

	glBindVertexArray( 0 );

	//GLenum err = glGetError( );

	//TODO: validate
	_valid = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
TechniqueScreenQuad::~TechniqueScreenQuad( )
{
	glDeleteProgram( _program );
	glDeleteBuffers( 1, &_vertex_buffer );
	glDeleteVertexArrays( 1, &_vertex_array );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
bool TechniqueScreenQuad::Draw( const GLuint texture )
{
	if (!_valid || !texture)
		return false;

	glUseProgram( _program );

	glBindVertexArray( _vertex_array );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );
	glUniform1i( _location_s0, 0 );

	glDrawArrays( GL_TRIANGLES, 0, 3 );

	glBindVertexArray( 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	//GLenum err = glGetError( );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void GLDrawScreenQuad( GLuint texture )
{
	static TechniqueScreenQuad sTechnique;
	sTechnique.Draw( texture );
}
