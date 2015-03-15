#include "program.h"


#ifndef IOS
#define USE_SHADER
#endif


#ifdef USE_SHADER


#include <vector>
#include <algorithm>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include "rays/exception.h"
#include "rays/shader.h"


namespace Rays
{


	class UniformValue
	{

		public:

			virtual ~UniformValue () {}

			virtual void apply (GLint location) const =0;

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

			void apply (GLint location) const;

		private:

			T array[DIMENSION];

	};// UniformValueT


	template <> void UniformValueT<int,   1>::apply (GLint location) const {glUniform1iv(location, 1, array);}

	template <> void UniformValueT<int,   2>::apply (GLint location) const {glUniform2iv(location, 1, array);}

	template <> void UniformValueT<int,   3>::apply (GLint location) const {glUniform3iv(location, 1, array);}

	template <> void UniformValueT<int,   4>::apply (GLint location) const {glUniform4iv(location, 1, array);}

	template <> void UniformValueT<float, 1>::apply (GLint location) const {glUniform1fv(location, 1, array);}

	template <> void UniformValueT<float, 2>::apply (GLint location) const {glUniform2fv(location, 1, array);}

	template <> void UniformValueT<float, 3>::apply (GLint location) const {glUniform3fv(location, 1, array);}

	template <> void UniformValueT<float, 4>::apply (GLint location) const {glUniform4fv(location, 1, array);}


	struct Uniform
	{

		struct Data
		{

			String name;

			boost::scoped_ptr<const UniformValue> value;

		};// Data

		Xot::PImpl<Data, true> self;

		Uniform (const char* name, const UniformValue* value)
		{
			if (!name || !value || name[0] == '\0')
				argument_error(__FILE__, __LINE__);

			self->name = name;
			self->value.reset(value);
		}

		void apply (GLuint program) const
		{
			GLint location = glGetUniformLocation(program, self->name);
			if (location < 0) return;

			self->value->apply(location);
			check_error(__FILE__, __LINE__);
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


	typedef std::vector<Shader>      ShaderList;

	typedef std::vector<ShaderList>  ShaderStack;

	typedef std::vector<Uniform>     UniformList;

	typedef std::vector<UniformList> UniformStack;


	struct Program::Data
	{

		int id;

		ShaderStack shader_stack;

		UniformStack uniform_stack;

		bool need_link, using_;

		Data ()
		:	id(-1), need_link(false), using_(false)
		{
			shader_stack.push_back(ShaderList());
			uniform_stack.push_back(UniformList());
		}

		~Data ()
		{
			clear();
		}

		void create ()
		{
			if (is_valid())
				invalid_state_error(__FILE__, __LINE__, "program is already created.");

			id = glCreateProgram();
		}

		void clear ()
		{
			if (shader_stack.size() != 1 || uniform_stack.size() != 1)
				invalid_state_error(__FILE__, __LINE__, "stack state is invalid.");

			ShaderList& list = shader_stack.back();
			for (ShaderList::iterator it = list.begin(); it != list.end(); ++it)
				detach(*it);

			if (id >= 0) glDeleteProgram((GLuint) id);

			id = -1;
		}

		void attach (const Shader& shader)
		{
			if (!is_valid()) create();

			ShaderList& list = shader_stack.back();
			ShaderList::iterator it = std::find(list.begin(), list.end(), shader);
			if (it != list.end()) return;

			glAttachShader(id, shader.id());
			check_error(__FILE__, __LINE__);

			list.push_back(shader);
			need_link = true;
		}

		void detach (const Shader& shader)
		{
			ShaderList& list = shader_stack.back();
			ShaderList::iterator it = std::find(list.begin(), list.end(), shader);
			if (it == list.end())
				rays_error(__FILE__, __LINE__, "shader is not attached.");

			glDetachShader(id, shader.id());
			check_error(__FILE__, __LINE__);

			list.erase(it);
			need_link = true;
		}

		void set_uniform (const char* name, const UniformValue* value)
		{
			if (uniform_stack.empty())
				invalid_state_error(__FILE__, __LINE__);

			UniformList& list = uniform_stack.back();
			std::remove_if(list.begin(), list.end(), UniformNameEqualFun(name));
			list.push_back(Uniform(name, value));

			if (using_) list.back().apply(id);
		}

		void update (bool uniforms = true)
		{
			link();

			use(!shader_stack.back().empty());
			if (using_ && uniforms) apply_all_uniforms();
		}

		bool is_valid () const
		{
			return id >= 0;
		}

		private:

			void link ()
			{
				if (!need_link) return;

				glLinkProgram(id);

				GLint status = GL_FALSE;
				glGetProgramiv(id, GL_LINK_STATUS, &status);
				if (status == GL_FALSE)
				{
					int len = 0;
					glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);

					boost::scoped_array<char> buffer(new char[len]);
					int written = 0;
					glGetProgramInfoLog(id, len, &written, &buffer[0]);

					opengl_error(__FILE__, __LINE__, &buffer[0]);
				}

				check_error(__FILE__, __LINE__);

				need_link = false;
			}

			void use (bool state)
			{
				if (state == using_) return;

				glUseProgram(state ? id : 0);
				using_ = state;
			}

			void apply_all_uniforms ()
			{
				if (uniform_stack.empty())
					invalid_state_error(__FILE__, __LINE__);

				UniformList& list = uniform_stack.back();
				for (UniformList::iterator it = list.begin(); it != list.end(); ++it)
					it->apply(id);
			}

			struct UniformNameEqualFun
			{

				String name;

				UniformNameEqualFun (const char* name)
				:	name(name)
				{
				}

				bool operator () (const Uniform& uniform) const
				{
					return uniform.self->name == name;
				}

			};// UniformNameEqualFun

	};// Program::Data


	Program::Program ()
	{
	}

	Program::~Program ()
	{
	}

	void
	Program::attach (const Shader& shader)
	{
		self->attach(shader);
		self->update();
	}

	void
	Program::detach (const Shader& shader)
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		self->detach(shader);
		self->update(false);
	}

	void
	Program::set_uniform (const char* name, int arg1)
	{
		self->set_uniform(name, new UniformValueT<int, 1>(arg1));
	}

	void
	Program::set_uniform (const char* name, int arg1, int arg2)
	{
		self->set_uniform(name, new UniformValueT<int, 2>(arg1, arg2));
	}

	void
	Program::set_uniform (const char* name, int arg1, int arg2, int arg3)
	{
		self->set_uniform(name, new UniformValueT<int, 3>(arg1, arg2, arg3));
	}

	void
	Program::set_uniform (const char* name, int arg1, int arg2, int arg3, int arg4)
	{
		self->set_uniform(name, new UniformValueT<int, 4>(arg1, arg2, arg3, arg4));
	}

	void
	Program::set_uniform (const char* name, const int* args, size_t size)
	{
		UniformValue* value = NULL;
		switch (size)
		{
			case 1: value = new UniformValueT<int, 1>(args, 1); break;
			case 2: value = new UniformValueT<int, 2>(args, 2); break;
			case 3: value = new UniformValueT<int, 3>(args, 3); break;
			case 4: value = new UniformValueT<int, 4>(args, 4); break;
		}

		if (value == NULL)
			argument_error(__FILE__, __LINE__, "invalid 'size' value.");

		self->set_uniform(name, value);
	}

	void
	Program::set_uniform (const char* name, float arg1)
	{
		self->set_uniform(name, new UniformValueT<float, 1>(arg1));
	}

	void
	Program::set_uniform (const char* name, float arg1, float arg2)
	{
		self->set_uniform(name, new UniformValueT<float, 2>(arg1, arg2));
	}

	void
	Program::set_uniform (const char* name, float arg1, float arg2, float arg3)
	{
		self->set_uniform(name, new UniformValueT<float, 3>(arg1, arg2, arg3));
	}

	void
	Program::set_uniform (const char* name, float arg1, float arg2, float arg3, float arg4)
	{
		self->set_uniform(name, new UniformValueT<float, 4>(arg1, arg2, arg3, arg4));
	}

	void
	Program::set_uniform (const char* name, const float* args, size_t size)
	{
		UniformValue* value = NULL;
		switch (size)
		{
			case 1: value = new UniformValueT<float, 1>(args, 1); break;
			case 2: value = new UniformValueT<float, 2>(args, 2); break;
			case 3: value = new UniformValueT<float, 3>(args, 3); break;
			case 4: value = new UniformValueT<float, 4>(args, 4); break;
		}

		if (value == NULL)
			argument_error(__FILE__, __LINE__, "invalid 'size' value.");

		self->set_uniform(name, value);
	}

	void
	Program::push ()
	{
		self->shader_stack.push_back(self->shader_stack.back());
		self->uniform_stack.push_back(self->uniform_stack.back());
	}

	static void
	pop_shader_stack (Program::Data* self)
	{
		if (!self)
			argument_error(__FILE__, __LINE__);

		if (self->shader_stack.size() < 2)
			invalid_state_error(__FILE__, __LINE__, "shader stack underflow.");

		size_t size = self->shader_stack.size();
		ShaderList& prev = self->shader_stack[size - 1];
		ShaderList& next = self->shader_stack[size - 2];

		ShaderList::iterator prev_end = prev.end();
		ShaderList::iterator next_end = next.end();

		for (ShaderList::iterator it = prev.begin(); it != prev_end; ++it)
		{
			ShaderList::iterator found = std::find(next.begin(), next_end, *it);
			if (found == next_end) self->detach(*it);
		}

		for (ShaderList::iterator it = next.begin(); it != next_end; ++it)
		{
			ShaderList::iterator found = std::find(prev.begin(), prev_end, *it);
			if (found == prev_end) self->attach(*it);
		}

		self->shader_stack.pop_back();
	}

	static void
	pop_uniform_stack (Program::Data* self)
	{
		if (!self)
			argument_error(__FILE__, __LINE__);

		if (self->uniform_stack.size() < 2)
			invalid_state_error(__FILE__, __LINE__, "uniform stack underflow.");

		size_t size = self->uniform_stack.size();
		UniformList& prev = self->uniform_stack[size - 1];
		UniformList& next = self->uniform_stack[size - 2];

		UniformList::iterator prev_end = prev.end();
		UniformList::iterator next_end = next.end();

		for (UniformList::iterator it = next.begin(); it != next_end; ++it)
		{
			UniformList::iterator found = std::find(prev.begin(), prev_end, *it);
			if (found == prev_end) it->apply(self->id);
		}

		self->uniform_stack.pop_back();
	}

	void
	Program::pop ()
	{
		pop_shader_stack(self.get());
		pop_uniform_stack(self.get());
		self->update();
	}

	GLuint
	Program::id () const
	{
		return self->id;
	}

	Program::operator bool () const
	{
		return self->is_valid();
	}

	bool
	Program::operator ! () const
	{
		return !operator bool();
	}


}// Rays


#else// USE_SHADER


#include "rays/exception.h"


namespace Rays
{


	struct Program::Data {};


	Program::Program ()
	{
	}

	Program::~Program ()
	{
	}

	void
	Program::attach (const Shader& shader)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::detach (const Shader& shader)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, int arg1)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, int arg1, int arg2)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, int arg1, int arg2, int arg3)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, int arg1, int arg2, int arg3, int arg4)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, const int* args, size_t size)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, float arg1)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, float arg1, float arg2)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, float arg1, float arg2, float arg3)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, float arg1, float arg2, float arg3, float arg4)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::set_uniform (const char* name, const float* args, size_t size)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Program::push ()
	{
	}

	void
	Program::pop ()
	{
	}

	GLuint
	Program::id () const
	{
		return 0;
	}

	Program::operator bool () const
	{
		return false;
	}

	bool
	Program::operator ! () const
	{
		return !operator bool();
	}


}// Rays


#endif// USE_SHADER
