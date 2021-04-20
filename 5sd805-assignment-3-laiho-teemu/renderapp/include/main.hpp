// main.hpp

#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

#include <avocado.hpp>
#include <avocado_render.hpp>

#include <camera.hpp>
#include "skybox.hpp"

#include "heightmap.hpp"

namespace avocado {
    //struct vertex {
    //    glm::vec3 position_;
    //};
   struct vertexcoordinatenormal {
      glm::vec3 position_;
      glm::vec2 texcoord_;
      glm::vec3 normal_;
   };

   struct chunk {
       int start_index_;
       int index_count_;

       glm::vec3 min_corner_;
       glm::vec3 max_corner_;
   };

   struct renderapp final : application {
      renderapp();

      virtual bool on_init();
      virtual void on_exit();
      virtual bool on_tick(const time &deltatime);
      virtual void on_draw();

      void set_phong_reflection_uniforms(int mode, int color);
      void change_light();

      renderer renderer_;
      shader_program shader_;
      vertex_buffer buffer_;
      vertex_layout layout_;
      texture texture_;
      texture texture2_;
      sampler_state sampler_;
      int32 vertex_count_;

      shader_program heightmap_shader_;
      index_buffer index_buffer_;
      vertex_buffer vertex_buffer_;
      vertex_layout vertex_layout_;

      glm::mat4 projection_;
      glm::mat4 world_;
      glm::mat4 world2_;
      glm::mat4 world3_;

      glm::mat4 sun_;

      camera camera_;
      frustum frustum_;
      controller controller_;

      heightmap heightmap_;
      uint32 heightmap_vertex_count;
      uint32 heightmap_vertex_size; 

      uint32 heightmap_index_count;
      uint32 heightmap_index_size;

      dynamic_array<vertex> vertices_;
      dynamic_array<uint32> indices_;

      skybox skybox_;

      float material_shininess_float;
      const void* material_shininess_pointer = &material_shininess_float;

      glm::vec3 lightdirection_;
      float deltatime_;
   };
} // !avocado
