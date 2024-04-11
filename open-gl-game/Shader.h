#pragma once

#include <string>
#include <optional>
#include <vector>


namespace gfx
{
	struct ShaderSource
	{
		std::string vertex;
		std::optional<std::string> control;
		std::optional<std::string> evaluation;
		std::optional<std::string> geometry;
		std::string fragment;
	};

	struct ShaderUniform
	{
		int id;
		int location;
		unsigned int  type;
	};

	struct ShaderVertexAttribute
	{
		// The type of this vertex attribute (eg. float3)
		GLenum		type;
		// The size of this vertex attribute, in bytes.
		GLenum		size;
		// The location of this vertex attribute (eg. location=0)
		GLint		location;
		// The name of this vertex attribute
		std::string	name;
	};

	extern ShaderSource default_shader_source;

	typedef unsigned int ShaderHandle;

	ShaderHandle CompileShader(const ShaderSource& source);
	void DeleteShader(ShaderHandle& handle);
	void UseShader(ShaderHandle handle);
	std::vector<ShaderVertexAttribute> GetShaderVertexAttributes(ShaderHandle handle);
	std::vector<ShaderUniform> GetShaderUniforms(ShaderHandle handle);
	GLint GetShaderUniformLocation(ShaderHandle handle, const std::string& name);
}