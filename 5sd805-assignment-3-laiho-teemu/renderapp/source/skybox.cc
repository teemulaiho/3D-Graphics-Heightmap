// skybox.cc

#include "skybox.hpp"

namespace avocado {
	skybox::skybox()
		: vertex_count_(0)
	{
	}

	bool skybox::create()
	{
		{ // note: load vertex and fragment shaders and create shader program
			string vertex_source;
			if (!file_system::read_file_content("assets/skybox/skybox.vs.txt", vertex_source)) {
				return false;
			}

			string fragment_source;
			if (!file_system::read_file_content("assets/skybox/skybox.fs.txt", fragment_source)) {
				return false;
			}

			if (!shader_.create(vertex_source.c_str(),
								fragment_source.c_str()))
			{
				return false;
			}
		}

		{ // note: create vertex buffer and layout
			const float Q = 2.0f;

			const glm::vec3 vertices[] =
			{
				// x positive
			  {  Q,  Q, -Q },
			  {  Q,  Q,  Q },
			  {  Q, -Q,  Q },
			  {  Q, -Q,  Q },
			  {  Q, -Q, -Q },
			  {  Q,  Q, -Q },

			  // x negative
			  { -Q,  Q,  Q },
			  { -Q,  Q, -Q },
			  { -Q, -Q, -Q },
			  { -Q, -Q, -Q },
			  { -Q, -Q,  Q },
			  { -Q,  Q,  Q },

			  // y positive
			  { -Q,  Q,  Q },
			  {  Q,  Q,  Q },
			  {  Q,  Q, -Q },
			  {  Q,  Q, -Q },
			  { -Q,  Q, -Q },
			  { -Q,  Q,  Q },

			  // y negative
			  { -Q, -Q, -Q },
			  {  Q, -Q, -Q },
			  {  Q, -Q,  Q },
			  {  Q, -Q,  Q },
			  { -Q, -Q,  Q },
			  { -Q, -Q, -Q },

			  // z positive
			  { -Q,  Q, -Q },
			  {  Q,  Q, -Q },
			  {  Q, -Q, -Q },
			  {  Q, -Q, -Q },
			  { -Q, -Q, -Q },
			  { -Q,  Q, -Q },

			  // z negative
			  {  Q,  Q,  Q },
			  { -Q,  Q,  Q },
			  { -Q, -Q,  Q },
			  { -Q, -Q,  Q },
			  {  Q, -Q,  Q },
			  {  Q,  Q,  Q },
			};
			
			// note: calclulate the number of elements in the vertices array
			vertex_count_ = sizeof(vertices) / sizeof(vertices[0]);

			// note: create the vertex buffer using vertices array
			if (!buffer_.create(BUFFER_ACCESS_MODE_STATIC, sizeof(vertices), vertices))
			{
				return false;
			}

			// note: specify the vertex layout
			layout_.add_attribute(0, vertex_layout::ATTRIBUTE_FORMAT_FLOAT, 3, false);
		}

		{ // note: load cubemap images and create cubemap
			const char* filenames[] =
			{
				"assets/skybox/xpos.jpg",
				"assets/skybox/xneg.jpg",
				"assets/skybox/ypos.jpg",
				"assets/skybox/yneg.jpg",
				"assets/skybox/zpos.jpg",
				"assets/skybox/zneg.jpg",
			};

			bitmap images[6];
			for (int32 index = 0; index < 6; index++)
			{
				if (!images[index].create(filenames[index]))
				{
					assert(!"could not load cubemap image");
				}
			}

			// note: verify image dimensions
			const int32 image_width = images[0].width();
			const int32 image_height = images[0].height();
			for (int32 index = 1; index < 6; index++) {
				assert(image_width == images[index].width());
				assert(image_height == images[index].height());
			}

			// note: pointers to image data
			const void* data[6] = {};
			for (int32 index = 0; index < 6; index++) {
				data[index] = images[index].data();
			}

			// note: create the cubemap
			const texture_format format = texture::from_bitmap_format(images[0].pixel_format());
			if (!cubemap_.create(format, image_width, image_height, data)) {
				return false;
			}

			// note: release image memory
			for (auto& image : images) {
				image.destroy();
			}

			if (!sampler_.create(	SAMPLER_FILTER_MODE_LINEAR,
									SAMPLER_ADDRESS_MODE_CLAMP,
									SAMPLER_ADDRESS_MODE_CLAMP,
									SAMPLER_ADDRESS_MODE_CLAMP))
			{
				return false;
			}
		}

		return true;
	}
	void skybox::destroy()
	{
		shader_.destroy();
		buffer_.destroy();
		cubemap_.destroy();
		sampler_.destroy();
	}

	void skybox::draw(renderer& rend, const camera& camera) 
	{
		glm::mat4 view = camera.view_;
		view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		rend.set_shader_program(shader_);
		rend.set_shader_uniform(shader_, UNIFORM_TYPE_MATRIX, "u_projection", 1, glm::value_ptr(camera.projection_));
		rend.set_shader_uniform(shader_, UNIFORM_TYPE_MATRIX, "u_view", 1, glm::value_ptr(view));
		rend.set_vertex_buffer(buffer_);
		rend.set_vertex_layout(layout_);
		rend.set_cubemap(cubemap_);
		rend.set_sampler_state(sampler_);
		rend.set_depth_state(true, true, 0.0f, 1.0f, COMPARE_FUNC_LESS_EQUAL);
		rend.set_rasterizer_state(CULL_MODE_FRONT);
		rend.draw(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, vertex_count_);
	}
} // !avocado
