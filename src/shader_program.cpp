#include "shader_program.h"


#include <assert.h>
#include <vector>
#include <memory>
#include <algorithm>
#include "rays/exception.h"
#include "shader_source.h"
#include "texture.h"
#include "painter.h"


namespace Rays
{


	static GLint
	get_texture_unit_max ()
	{
		static GLint value = -1;
		if (value < 0)
		{
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &value);
			check_error(__FILE__, __LINE__);
		}
		return value;
	}


	class UniformValue
	{

		public:

			virtual ~UniformValue () {}

			virtual void apply (GLint location) const = 0;

	};// UniformValue


	template <typename T, int DIMENSION>
	class UniformValueT : public UniformValue
	{

		public:

			UniformValueT (T arg1)
			{
				assert(DIMENSION == 1);
				array[0] = arg1;
			}

			UniformValueT (T arg1, T arg2)
			{
				assert(DIMENSION == 2);
				array[0] = arg1;
				array[1] = arg2;
			}

			UniformValueT (T arg1, T arg2, T arg3)
			{
				assert(DIMENSION == 3);
				array[0] = arg1;
				array[1] = arg2;
				array[2] = arg3;
			}

			UniformValueT (T arg1, T arg2, T arg3, T arg4)
			{
				assert(DIMENSION == 4);
				array[0] = arg1;
				array[1] = arg2;
				array[2] = arg3;
				array[3] = arg4;
			}

			UniformValueT (const T* args, size_t size)
			{
				assert(size == DIMENSION);
				for (size_t i = 0; i < size; ++i)
					array[i] = args[i];
			}

			void apply (GLint location) const
			{
				apply_value(location);
				check_error(__FILE__, __LINE__);
			}

			void apply_value (GLint location) const;

		private:

			T array[DIMENSION];

	};// UniformValueT


	template <> void UniformValueT<int, 1>::apply_value (GLint location) const
	{
		glUniform1iv(location, 1, array);
	}

	template <> void UniformValueT<int, 2>::apply_value (GLint location) const
	{
		glUniform2iv(location, 1, array);
	}

	template <> void UniformValueT<int, 3>::apply_value (GLint location) const
	{
		glUniform3iv(location, 1, array);
	}

	template <> void UniformValueT<int, 4>::apply_value (GLint location) const
	{
		glUniform4iv(location, 1, array);
	}

	template <> void UniformValueT<float, 1>::apply_value (GLint location) const
	{
		glUniform1fv(location, 1, array);
	}

	template <> void UniformValueT<float, 2>::apply_value (GLint location) const
	{
		glUniform2fv(location, 1, array);
	}

	template <> void UniformValueT<float, 3>::apply_value (GLint location) const
	{
		glUniform3fv(location, 1, array);
	}

	template <> void UniformValueT<float, 4>::apply_value (GLint location) const
	{
		glUniform4fv(location, 1, array);
	}


	class UniformTexture : public UniformValue
	{

		public:

			UniformTexture (const Texture& texture)
			:	texture(texture)
			{
			}

			void apply (GLint location) const
			{
				if (!texture) return;

				GLint unit_max = get_texture_unit_max();
				GLint unit = unit_max;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &unit);
				assert(unit < unit_max);

				glBindTexture(GL_TEXTURE_2D, texture.id());
				glUniform1i(location, unit);
				check_error(__FILE__, __LINE__);
			}

		private:

			Texture texture;

	};// UniformTexture


	struct Uniform
	{

		struct Data
		{

			String name;

			std::unique_ptr<const UniformValue> value;

			bool applied = false;

		};// Data

		Xot::PSharedImpl<Data> self;

		Uniform (const char* name, const UniformValue* value)
		{
			if (!name || name[0] == '\0')
				argument_error(__FILE__, __LINE__);

			reset(value);

			self->name = name;
		}

		void reset (const UniformValue* value)
		{
			if (!value)
				argument_error(__FILE__, __LINE__);

			self->value.reset(value);
			self->applied = false;
		}

		void apply (const ShaderProgram& program) const
		{
			if (!program || self->applied) return;
			self->applied = true;

			GLint location = glGetUniformLocation(program.id(), self->name);
			if (location < 0) return;

			self->value->apply(location);
		}

		bool operator == (const Uniform& rhs) const
		{
			return self.get() == rhs.self.get();
		}

		bool operator != (const Uniform& rhs) const
		{
			return !operator==(rhs);
		}

	};// Uniform


	typedef std::vector<ShaderSource> SourceList;

	typedef std::vector<Uniform> UniformList;


	struct ShaderProgram::Data
	{

		GLuint id = 0;

		SourceList sources;

		UniformList uniform_values, uniform_textures;

		mutable bool linked = false, applied = false;

		Data ()
		{
			id = glCreateProgram();
			if (id <= 0)
				opengl_error(__FILE__, __LINE__, "failed to create program.");
		}

		~Data ()
		{
			clear();
		}

		void clear ()
		{
			if (id > 0) glDeleteProgram(id);

			sources.clear();
			uniform_values.clear();
			uniform_textures.clear();

			id = 0;
			linked = applied = false;
		}

		void set_uniform_value (const char* name, const UniformValue* value)
		{
			set_uniform(&uniform_values, name, value);
		}

		void set_uniform_texture (const char* name, const UniformValue* value)
		{
			if (uniform_textures.size() >= (size_t) get_texture_unit_max())
				opengl_error(__FILE__, __LINE__, "too many texture units.");

			set_uniform(&uniform_textures, name, value);
		}

		void set_uniform (
			UniformList* uniforms, const char* name, const UniformValue* value)
		{
			assert(uniforms);

			auto it = std::find_if(
				uniforms->begin(), uniforms->end(), [&](const Uniform& uniform) {
					return uniform.self->name == name;
				});

			if (it != uniforms->end())
				it->reset(value);
			else
				uniforms->push_back(Uniform(name, value));

			applied = false;
		}

		bool is_valid () const
		{
			return id > 0 && !sources.empty();
		}

		void link_sources () const
		{
			if (linked) return;
			linked = true;

			const ShaderSource& vertex_shader = Painter_get_vertex_shader_source();

			glAttachShader(id, vertex_shader.id());
			for (const auto& fragment_shader : sources)
				glAttachShader(id, fragment_shader.id());
			check_error(__FILE__, __LINE__);

			glLinkProgram(id);
			check_error(__FILE__, __LINE__);

			glDetachShader(id, vertex_shader.id());
			for (const auto& fragment_shader : sources)
				glDetachShader(id, fragment_shader.id());
			check_error(__FILE__, __LINE__);

			GLint status = GL_FALSE;
			glGetProgramiv(id, GL_LINK_STATUS, &status);
			if (status == GL_FALSE)
				opengl_error(__FILE__, __LINE__, get_link_log().c_str());
		}

		String get_link_log () const
		{
			int len = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
			if (len <= 0) return "";

			std::unique_ptr<char[]> buffer(new char[len]);
			int written = 0;
			glGetProgramInfoLog(id, len, &written, &buffer[0]);
			return &buffer[0];
		}

		void apply_uniforms (const ShaderProgram& program) const
		{
			if (applied) return;
			applied = true;

			for (const auto& value : uniform_values)
				value.apply(program);

			int unit = 0;
			for (const auto& texture : uniform_textures)
			{
				glActiveTexture(unit++);
				texture.apply(program);
			}

			check_error(__FILE__, __LINE__);
		}

	};// ShaderProgram::Data


	void
	ShaderProgram_activate (const ShaderProgram& program)
	{
		ShaderProgram::Data* self = program.self.get();

		if (!self->is_valid()) return;

		self->link_sources();

		glUseProgram(program.id());
		check_error(__FILE__, __LINE__);

		self->apply_uniforms(program);
	}

	void
	ShaderProgram_deactivate ()
	{
		glUseProgram(0);
		check_error(__FILE__, __LINE__);
	}


	ShaderProgram::ShaderProgram ()
	{
	}

	ShaderProgram::~ShaderProgram ()
	{
	}

	void
	ShaderProgram::add_source (const ShaderSource& source)
	{
		self->sources.push_back(source);
		self->linked = false;
	}

	void
	ShaderProgram::remove_source (const ShaderSource& source)
	{
		auto it = std::find(self->sources.begin(), self->sources.end(), source);
		if (it == self->sources.end()) return;

		self->sources.erase(it);
		self->linked = false;
	}

	void
	ShaderProgram::set_uniform (const char* name, int arg1)
	{
		self->set_uniform_value(name, new UniformValueT<int, 1>(arg1));
	}

	void
	ShaderProgram::set_uniform (const char* name, int arg1, int arg2)
	{
		self->set_uniform_value(name, new UniformValueT<int, 2>(arg1, arg2));
	}

	void
	ShaderProgram::set_uniform (const char* name, int arg1, int arg2, int arg3)
	{
		self->set_uniform_value(name, new UniformValueT<int, 3>(arg1, arg2, arg3));
	}

	void
	ShaderProgram::set_uniform (
		const char* name, int arg1, int arg2, int arg3, int arg4)
	{
		self->set_uniform_value(
			name, new UniformValueT<int, 4>(arg1, arg2, arg3, arg4));
	}

	template <typename T>
	static UniformValue*
	create_uniform_value (const T* args, size_t size)
	{
		switch (size)
		{
			case 1: return new UniformValueT<T, 1>(args, 1);
			case 2: return new UniformValueT<T, 2>(args, 2);
			case 3: return new UniformValueT<T, 3>(args, 3);
			case 4: return new UniformValueT<T, 4>(args, 4);

			default:
				argument_error(__FILE__, __LINE__, "invalid 'size' value.");
		}

		return NULL;
	}

	void
	ShaderProgram::set_uniform (const char* name, const int* args, size_t size)
	{
		self->set_uniform_value(name, create_uniform_value(args, size));
	}

	void
	ShaderProgram::set_uniform (const char* name, float arg1)
	{
		self->set_uniform_value(name, new UniformValueT<float, 1>(arg1));
	}

	void
	ShaderProgram::set_uniform (const char* name, float arg1, float arg2)
	{
		self->set_uniform_value(name, new UniformValueT<float, 2>(arg1, arg2));
	}

	void
	ShaderProgram::set_uniform (const char* name, float arg1, float arg2, float arg3)
	{
		self->set_uniform_value(
			name, new UniformValueT<float, 3>(arg1, arg2, arg3));
	}

	void
	ShaderProgram::set_uniform (
		const char* name, float arg1, float arg2, float arg3, float arg4)
	{
		self->set_uniform_value(
			name, new UniformValueT<float, 4>(arg1, arg2, arg3, arg4));
	}

	void
	ShaderProgram::set_uniform (const char* name, const float* args, size_t size)
	{
		self->set_uniform_value(name, create_uniform_value(args, size));
	}

	void
	ShaderProgram::set_uniform (const char* name, const Coord2& vec2)
	{
		self->set_uniform_value(name, new UniformValueT<float, 2>(vec2.array, 2));
	}

	void
	ShaderProgram::set_uniform (const char* name, const Coord3& vec3)
	{
		self->set_uniform_value(name, new UniformValueT<float, 3>(vec3.array, 3));
	}

	void
	ShaderProgram::set_uniform (const char* name, const Coord4& vec4)
	{
		self->set_uniform_value(name, new UniformValueT<float, 4>(vec4.array, 4));
	}

	void
	ShaderProgram::set_uniform (const char* name, const Texture& texture)
	{
		self->set_uniform_texture(name, new UniformTexture(texture));
	}

	GLuint
	ShaderProgram::id () const
	{
		return self->id;
	}

	ShaderProgram::operator bool () const
	{
		return self->is_valid();
	}

	bool
	ShaderProgram::operator ! () const
	{
		return !operator bool();
	}

	bool
	operator == (const ShaderProgram& lhs, const ShaderProgram& rhs)
	{
		return (!lhs && !rhs) || lhs.self->id == rhs.self->id;
	}

	bool
	operator != (const ShaderProgram& lhs, const ShaderProgram& rhs)
	{
		return !operator==(lhs, rhs);
	}


}// Rays
