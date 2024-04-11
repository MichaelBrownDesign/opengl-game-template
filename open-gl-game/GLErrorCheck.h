#if _DEBUG
#pragma once
#include <GL/glew.h>
#include <iostream>

namespace
{
	GLenum error;
};

#define GL_ERRORCHECK()												\
while ((::error = glGetError()) != GL_NO_ERROR)						\
{																	\
	std::cerr << "[OpenGL] ("	<< __FILE__		<<					\
				": "			<< __LINE__		<<					\
				") Error #"		<< ::error		<<					\
				" : "			<< glewGetErrorString(::error) <<	\
				std::endl;											\
}
#else
#define GL_ERRORCHECK() (void)0
#endif