#include "frame_buffer.h"


#include <vector>
#include "rays/exception.h"
#include "rays/debug.h"
#include "texture.h"
#include "render_buffer.h"


namespace Rays
{


	struct FrameBuffer::Data
	{

		int id;

		Texture texture;

		RenderBuffer render_buffer;

		Data ()
		:	id(-1)
		{
		}

		~Data ()
		{
			clear();
		}

		void create ()
		{
			if (is_valid()) return;

			GLuint id_ = 0;
			glGenFramebuffers(1, &id_);
			check_error(__FILE__, __LINE__);

			id = id_;
		}

		void clear ()
		{
			texture       = Texture();
			render_buffer = RenderBuffer();

			if (id >= 0)
			{
				GLuint id_ = id;
				glDeleteFramebuffers(1, &id_);
				check_error(__FILE__, __LINE__);
			}

			id = -1;
		}

		bool is_valid () const
		{
			return id >= 0;
		}

	};// FrameBuffer::Data


	FrameBuffer::FrameBuffer ()
	{
	}

	FrameBuffer::FrameBuffer (const Texture& texture)
	{
		attach(texture);
	}

	FrameBuffer::~FrameBuffer ()
	{
	}

	static String
	get_error_status_name (GLenum status)
	{
		switch (status)
		{
			case GL_FRAMEBUFFER_UNSUPPORTED:                   return "GL_FRAMEBUFFER_UNSUPPORTED";
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
			#ifdef IOS
				case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
			#else
				case GL_FRAMEBUFFER_UNDEFINED:                  return "GL_FRAMEBUFFER_UNDEFINED";
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:     return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:     return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
				//case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:   return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
			#endif
		}
		return "UNKNOWN STATUS";
	}

	void
	FrameBuffer::attach (const Texture& texture)
	{
		if (!texture)
			argument_error(__FILE__, __LINE__, "FrameBuffer::attach(): invalid texture.");

		self->create();

		FrameBufferBinder binder(id());

		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.id(), 0);
		check_error(__FILE__, __LINE__);

		self->texture = texture;

#if 0
		int w = texture.width(), h = texture.height();
		if (
			w != self->render_buffer.width() ||
			h != self->render_buffer.height())
		{
			RenderBuffer rb = RenderBuffer(w, h);

			glFramebufferRenderbuffer(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, rb.id());
			check_error(__FILE__, __LINE__);

			self->render_buffer = rb;
		}
#endif

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			opengl_error(__FILE__, __LINE__,
				"frame buffer status is %s", get_error_status_name(status).c_str());
		}
	}

	GLuint
	FrameBuffer::id () const
	{
		return self->id;
	}

	int
	FrameBuffer::width () const
	{
		return self->texture.width();
	}

	int
	FrameBuffer::height () const
	{
		return self->texture.height();
	}

	Texture&
	FrameBuffer::texture ()
	{
		return self->texture;
	}

	const Texture&
	FrameBuffer::texture () const
	{
		return const_cast<FrameBuffer*>(this)->texture();
	}

	FrameBuffer::operator bool () const
	{
		if (!self || !self->is_valid())
			return false;

		const Texture& t = self->texture;
		if (!t)
			return false;

		const RenderBuffer& r = self->render_buffer;
		if (r && (t.width() != r.width() || t.height() != r.height()))
			return false;

		return true;
	}

	bool
	FrameBuffer::operator ! () const
	{
		return !operator bool();
	}


	FrameBufferBinder::FrameBufferBinder (GLuint id)
	:	id(id)
	{
		bind_frame_buffer(id);
	}

	FrameBufferBinder::~FrameBufferBinder ()
	{
		unbind_frame_buffer();
	}

	GLuint
	FrameBufferBinder::current () const
	{
		return id;
	}


	static std::vector<GLuint> frame_buffer_bind_stack;


	void
	bind_frame_buffer (GLuint id)
	{
		GLuint current = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &current);
		check_error(__FILE__, __LINE__);

		frame_buffer_bind_stack.push_back(current);

		glBindFramebuffer(GL_FRAMEBUFFER, id);
		check_error(__FILE__, __LINE__);
	}

	void
	unbind_frame_buffer ()
	{
		if (frame_buffer_bind_stack.empty())
			rays_error(__FILE__, __LINE__, "frame_buffer_bind_stack underflow.");

		GLuint id = frame_buffer_bind_stack.back();
		frame_buffer_bind_stack.pop_back();

		glBindFramebuffer(GL_FRAMEBUFFER, id);
		check_error(__FILE__, __LINE__);
	}


}// Rays
