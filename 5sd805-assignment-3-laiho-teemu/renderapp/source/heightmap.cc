// heightmap.cc
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#pragma warning(pop)

#include "heightmap.hpp"

namespace avocado {
	heightmap::heightmap()
		: image_width(0)
		, image_height(0)	
		, vertex_count(0)
		, index_count(0)
	{
	}

	bool heightmap::create(dynamic_array<vertex>& vertices, dynamic_array<uint32>& indices)
	{
		//dynamic_array<vertex> vertices;
		//dynamic_array<uint32> indices;

		// note: load heightmap image and create heightmap
		{
			const char* filenames[] =
			{
				//"assets/heightmap/heightmap_m.png",
				"assets/heightmap/TKInverted.png",
			};

			bitmap image;
			if (!image.create(filenames[0]))
			{
				assert(!"could not load heightmap image");
			}

			// note: verify image dimensions
			{
				image_width = image.width();
				image_height = image.height();

				if (image_width != 256 || image_height != 256)
				{
					assert("heightmap dimensions not correct!");
				}

				// note: pointer to image data
				const void* data = {};
				data = image.data();
			}

			// note: set vertex buffer
			{
				vertex_count = image_width * image_height;
				vertices.resize(vertex_count);

				// note: © Tommi Lipponen - 5SD805: Real-time Graphics Programming for Games 1 - 2020
				for (int32 index = 0; index < vertex_count; index++) {
					const int32 x = index % image_width;
					const int32 z = index / image_width;
					int32 pixel = image.get_pixel(x, z);

					// 0xAABBGGRR
					// 0xff = R
					// 0xff00 = G
					// 0xff0000 = B
					// 0xff000000 = A
					pixel = pixel & 0xff; // 0xff = 0x00 00 00 00 

					//pixel = 0;

					//const int32 y = index;
					vertices[index].position_.x = static_cast<float>(x);
					vertices[index].position_.y = static_cast<float>((255 - pixel) / 10);
					vertices[index].position_.z = static_cast<float>(z);

					//vertices[index].texcoord_ = glm::vec2(0.0f, 0.0f);
					//vertices[index].normal_ = glm::vec3(0.0f, 0.0f, 1.0f);

					vertices[index].color_.a = 1.0f;
					vertices[index].color_.b = 0.0f;
					vertices[index].color_.g = 0.0f;
					vertices[index].color_.r = 1.0f;
				}
			}

			// note: set index buffer
			{
				index_count = (image_width - 1) * (image_height - 1) * 6;
				//indices.resize(index_count);
				indices.reserve(index_count);

				// chunkification algorithm.
				for (int y = 0; y < image_height / 7; y++)
				{
					for (int x = 0; x < image_width / 7; x++)
					{
						for (int base_y = 0; base_y < 7; base_y++)
						{
							for (int32 base_x = 0; base_x < 7; base_x++)
							{
								// chunk row * offset + indices row * 
								int base = (y * 7 + base_y) * image_width + 7 * x + base_x;	

								// Triangle 1 values
								indices.push_back(base);
								indices.push_back(base + image_width);
								indices.push_back(base + image_width + 1);

								// Triangle 2 values
								indices.push_back(base + image_width + 1);
								indices.push_back(base + 1);
								indices.push_back(base);
							}
						}
					}
				}		

			}

			// note: set vertex normals
			{
				for (int32 index = 0; index < indices.size() - 3; index += 3)
				{
					// note: get triangle normals
					glm::vec3 normal = getsurfacenormal(vertices[indices[index]].position_, 
														 vertices[indices[index+1]].position_, 
														 vertices[indices[index+2]].position_);
					vertices[indices[index]].normal_ = normal;
				}
			}

			// note: release image memory
			image.destroy();
		}
		return true;
	}

	glm::vec3 heightmap::getsurfacenormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
	{
		glm::vec3 v10 = v1 - v0;
		glm::vec3 v20 = v2 - v0;
		glm::vec3 c = glm::cross(v10, v20);
		glm::vec3 n = glm::normalize(c);

		return c;
	}
}
