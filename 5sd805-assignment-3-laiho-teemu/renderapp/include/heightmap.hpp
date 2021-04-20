// heightmap.hpp

#ifndef HEIGHTMAP_HPP_INCLUDED
#define HEIGHTMAP_HPP_INCLUDED

#include <avocado.hpp>
#include <avocado_render.hpp>

namespace avocado {
	struct vertex {
		glm::vec3 position_;
		glm::vec4 color_;
		glm::vec3 normal_;
	};

	struct heightmap {
		heightmap();

		bool create(dynamic_array<vertex> &vertices, dynamic_array<uint32> &indices);

		glm::vec3 getsurfacenormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

		int32 image_width;
		int32 image_height;
		int32 vertex_count;
		int32 index_count;
	};
}


#endif // !HEIGHTMAP_HPP_INCLUDED