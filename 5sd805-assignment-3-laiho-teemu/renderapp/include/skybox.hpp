// skybox.hpp

#ifndef SKYBOX_HPP_INCLUDED
#define SKYBOX_HPP_INCLUDED

#include <avocado.hpp>
#include <avocado_render.hpp>

#include "camera.hpp"

namespace avocado {
	struct skybox {
		skybox();

		bool create();
		void destroy();

		void draw(renderer &rend, const camera &camera);

		shader_program shader_;
		vertex_buffer buffer_;
		vertex_layout layout_;
		cubemap cubemap_;
		sampler_state sampler_;
		int32 vertex_count_;
	};
}	// !avocado

#endif // !SKYBOX_HPP_INCLUDED

