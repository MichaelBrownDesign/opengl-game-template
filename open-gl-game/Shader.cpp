#include <GL/glew.h>
#include <functional>
#include "shader.h"

namespace gfx
{
	std::hash<std::string> string_hash;

	ShaderSource default_shader_source =
	{
		"#version 330 core \n"
		"layout(location = 0) in vec3 position; \n"
		"layout(location = 1) in vec2 uv; \n"
		"uniform mat4 mvp; \n"
		"out VS_OUT { \n"
		"vec2 uv; \n"
		"} vs_out; \n"
		""
		"void main() { \n"
		"vs_out.uv = uv; \n"
		"gl_Position = mvp * vec4(position, 1.0); \n"
		"}",

		std::optional<std::string>(),
		std::optional<std::string>(),
		std::optional<std::string>(),

		"#version 330 core \n"
		"uniform sampler2D textureMap; \n"
		"in VS_OUT { \n"
		"vec2 uv; \n"
		"} fs_in; \n"
		"out vec4 fragment; \n"
		"void main() { \n"
		"fragment = texture(textureMap, fs_in.uv); \n"
		"if(fragment.a < 0.5) discard; \n"
		//"fragment = vec4(1,0,0,1); \n"
		"} \n"
	};

	bool compile_shader_source(GLenum type, GLsizei count, const std::string& source, GLuint& shaderHandle)
	{
		shaderHandle = glCreateShader(type);

		int compiled = 0;

		const char* src = source.c_str();
		const int length = source.length();
		glShaderSource(shaderHandle, count, &src, &length);
		glCompileShader(shaderHandle);
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);

		return static_cast<bool>(compiled);
	}

	void cleanup_shaders(GLuint vertex, GLuint control, GLuint evaluation, GLuint geometry, GLuint fragment)
	{
		if (vertex)		glDeleteShader(vertex);
		if (control)		glDeleteShader(control);
		if (evaluation)	glDeleteShader(evaluation);
		if (geometry)	glDeleteShader(geometry);
		if (fragment)	glDeleteShader(fragment);
	}

	ShaderHandle CompileShader(const ShaderSource& source)
	{
		GLuint programHandle = glCreateProgram();

		GLuint vertexShader = 0;
		GLuint controlShader = 0;
		GLuint evaluationShader = 0;
		GLuint geometryShader = 0;
		GLuint fragmentShader = 0;

		if (!compile_shader_source(GL_VERTEX_SHADER, 1, source.vertex, vertexShader))
		{
			cleanup_shaders(vertexShader, controlShader, evaluationShader, geometryShader, fragmentShader);
			return 0;
		}

		if (!compile_shader_source(GL_FRAGMENT_SHADER, 1, source.fragment, fragmentShader))
		{
			cleanup_shaders(vertexShader, controlShader, evaluationShader, geometryShader, fragmentShader);
			return 0;
		}

		if (source.control.has_value())
		{
			if (!compile_shader_source(GL_TESS_CONTROL_SHADER, 1, source.control.value(), controlShader))
			{
				cleanup_shaders(vertexShader, controlShader, evaluationShader, geometryShader, fragmentShader);
				return 0;
			}
		}

		if (source.evaluation.has_value())
		{
			if (!compile_shader_source(GL_TESS_EVALUATION_SHADER, 1, source.evaluation.value(), evaluationShader))
			{
				cleanup_shaders(vertexShader, controlShader, evaluationShader, geometryShader, fragmentShader);
				return 0;
			}
		}

		if (source.geometry.has_value())
		{
			if (!compile_shader_source(GL_GEOMETRY_SHADER, 1, source.geometry.value(), geometryShader))
			{
				cleanup_shaders(vertexShader, controlShader, evaluationShader, geometryShader, fragmentShader);
				return 0;
			}
		}

		glAttachShader(programHandle, vertexShader);
		glAttachShader(programHandle, fragmentShader);

		if (source.control.has_value())		glAttachShader(programHandle, controlShader);
		if (source.evaluation.has_value())	glAttachShader(programHandle, evaluationShader);
		if (source.geometry.has_value())	glAttachShader(programHandle, geometryShader);

		glLinkProgram(programHandle);

		glDetachShader(programHandle, vertexShader);
		glDetachShader(programHandle, fragmentShader);

		if (source.control.has_value())		glDetachShader(programHandle, controlShader);
		if (source.evaluation.has_value())	glDetachShader(programHandle, evaluationShader);
		if (source.geometry.has_value())	glDetachShader(programHandle, geometryShader);

		cleanup_shaders(vertexShader, controlShader, evaluationShader, geometryShader, fragmentShader);

		GLint linked = 0;
		glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			return 0;
		}

		return programHandle;
	}

	void DeleteShader(ShaderHandle& handle)
	{
		glDeleteProgram(handle);
		handle = 0;
	}

	void UseShader(ShaderHandle handle)
	{
		glUseProgram(handle);
	}

	std::vector<ShaderVertexAttribute> GetShaderVertexAttributes(ShaderHandle handle)
	{
		GLint attributeCount = 0;
		GLint attributeMaxLength = 0;

		glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);
		glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeMaxLength);

		std::vector<ShaderVertexAttribute> attributes(attributeCount);
		GLint sizeWritten = 0;
		GLint attributeSize = 0;
		GLenum attributeType = 0;
		GLchar* name = new GLchar[attributeMaxLength];

		for (GLuint i = 0; i < attributeCount; ++i)
		{
			glGetActiveAttrib(handle, i, attributeMaxLength, &sizeWritten, &attributeSize, &attributeType, name);
			GLint location = glGetAttribLocation(handle, name);

			ShaderVertexAttribute attr;
			attr.type = attributeType;
			attr.size = attributeSize;
			attr.location = location;
			attr.name = name;
			attributes[i] = attr;
		}

		delete[] name;
		return attributes;
	}

	std::vector<ShaderUniform> GetShaderUniforms(ShaderHandle handle)
	{
		GLint uniformCount;
		GLint uniformMaxLength;

		glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &uniformCount);
		glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);

		std::vector<ShaderUniform> uniforms(uniformCount);

		GLchar* glName = new GLchar[uniformMaxLength];
		GLsizei written = 0;
		GLint size;
		GLint location;
		GLenum type;

		for (GLuint i = 0; i < uniformCount; ++i)
		{
			glGetActiveUniform(handle, i, uniformMaxLength, &written, &size, &type, glName);
			location = glGetUniformLocation(handle, glName);

			std::string name(glName, written);

			ShaderUniform uniform;
			uniform.id = string_hash(name);
			uniform.location = location;
			uniform.type = type;
			uniforms.push_back(uniform);
		}

		delete[] glName;

		return uniforms;
	}

	GLint GetShaderUniformLocation(ShaderHandle handle, const std::string& name)
	{
		const GLchar* glName = static_cast<const GLchar*>(name.c_str());
		GLint location = glGetUniformLocation(handle, glName);
		return location;
	}
}