// main.cc

// Controls:
// Wireframe mode: hold "t".

#include "main.hpp"

#include "avocado_render.hpp"
#include "avocado_opengl.h"

namespace avocado {
   // note: camera
  

   // note: application create implementation
   application *application::create(settings &settings)
   {
      settings.title_      = "renderapp";
      settings.width_      = 1280;
      settings.height_     = 720;
      settings.center_     = true;
      //settings.borderless_ = true;

      return new renderapp;
   }

   // note: renderapp class
   renderapp::renderapp()
      : controller_(camera_)
   {
   }

   bool renderapp::on_init()
   {
      if (!renderer_.initialize()) {
         return false;
      }

      // note: set default light direction
      lightdirection_ = glm::vec3{ 0.0f, 10.0f,0.0f };

      // note: create skybox
      {
          if (!skybox_.create()) {
              return on_error("could not create skybox!");
          }
      }

      // note: create heightmap
      {
          heightmap_.create(vertices_, indices_);

          heightmap_vertex_size = static_cast<uint32>(vertices_.size() * sizeof(vertex));
          heightmap_vertex_count = static_cast<uint32>(vertices_.size());

          heightmap_index_size = static_cast<uint32>(indices_.size() * sizeof(uint32));
          heightmap_index_count = static_cast<uint32>(indices_.size());

          // note: © Tommi Lipponen - 5SD805: Real-time Graphics Programming for Games 1 - 2020
          if (!index_buffer_.create(heightmap_index_size, indices_.data())) {
              return on_error("could not create index buffer");
          }

          // note: © Tommi Lipponen - 5SD805: Real-time Graphics Programming for Games 1 - 2020
          if (!vertex_buffer_.create(BUFFER_ACCESS_MODE_STATIC,
              heightmap_vertex_size,
              vertices_.data()))
          {
              return on_error("could not create terrain vertex buffer");
          }
      }

      // note: load shader source from disk
      {
         string vertex_source;
         if (!file_system::read_file_content("assets/triangle.vs.txt", vertex_source)) {
            return on_error("Could not load vertex source");
         }

         string fragment_source;
         if (!file_system::read_file_content("assets/triangle.fs.txt", fragment_source)) {
            return on_error("Could not load fragment source");
         }

         if (!shader_.create(vertex_source.c_str(),
                             fragment_source.c_str()))
         {
            return on_error("Could not create shader program!");
         }
      }

      // note: load heightmap shader source from disk
      {
          string vertex_source;
          if (!file_system::read_file_content("assets/heightmap/heightmap.vs.txt", vertex_source)) {
              return on_error("Could not load vertex source");
          }

          string fragment_source;
          if (!file_system::read_file_content("assets/heightmap/heightmap.fs.txt", fragment_source)) {
              return on_error("Could not load fragment source");
          }

          if (!heightmap_shader_.create(vertex_source.c_str(),
              fragment_source.c_str()))
          {
              return on_error("Could not create shader program!");
          }
      }

      // note: create cube
      {
         const float v = 0.5f;

         const glm::vec3 p[] =
         {
            { -v, +v, +v },
            { -v, -v, +v },
            { +v, -v, +v },
            { +v, +v, +v },

            { -v, +v, -v },
            { -v, -v, -v },
            { +v, -v, -v },
            { +v, +v, -v },
         };

         const vertexcoordinatenormal vertices[] =
         {
            // front
            { p[0], glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), },
            { p[1], glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), },
            { p[2], glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), },
            { p[2], glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), },
            { p[3], glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), },
            { p[0], glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), },
            // right                                  
            { p[3], glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), },
            { p[2], glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), },
            { p[6], glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), },
            { p[6], glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), },
            { p[7], glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), },
            { p[3], glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), },
            // back                                   
            { p[7], glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f), },
            { p[6], glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f,-1.0f), },
            { p[5], glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f,-1.0f), },
            { p[5], glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f,-1.0f), },
            { p[4], glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f), },
            { p[7], glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f), },
            // left
            { p[4], glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), },
            { p[5], glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), },
            { p[1], glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), },
            { p[1], glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), },
            { p[0], glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), },
            { p[4], glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), },
            // top
            { p[4], glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), },
            { p[0], glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), },
            { p[3], glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), },
            { p[3], glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), },
            { p[7], glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), },
            { p[4], glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), },
            // bottom                                 
            { p[1], glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f), },
            { p[5], glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f), },
            { p[6], glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f), },
            { p[6], glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f), },
            { p[2], glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f), },
            { p[1], glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f), },
         };

         vertex_count_ = sizeof(vertices) / sizeof(vertices[0]);

         if (!buffer_.create(BUFFER_ACCESS_MODE_STATIC, sizeof(vertices), vertices)) {
            return on_error("could not create vertex buffer!");
         }
      }

      // note: describe vertex layout_
      {
         layout_.add_attribute(0, vertex_layout::ATTRIBUTE_FORMAT_FLOAT, 3, false);
         layout_.add_attribute(1, vertex_layout::ATTRIBUTE_FORMAT_FLOAT, 2, false);
         layout_.add_attribute(2, vertex_layout::ATTRIBUTE_FORMAT_FLOAT, 3, false);
      }

      // note: describe heightmap vertex_layout_
      {
          vertex_layout_.add_attribute(0, vertex_layout::ATTRIBUTE_FORMAT_FLOAT, 3, false);
          vertex_layout_.add_attribute(1, vertex_layout::ATTRIBUTE_FORMAT_FLOAT, 4, false);
          vertex_layout_.add_attribute(2, vertex_layout::ATTRIBUTE_FORMAT_FLOAT, 3, false);
      }

      // note: load bitmap and create texture
      {
         bitmap image;
         if (!image.create("assets/crate.png")) {
            return on_error("could not load bitmap image!");
         }

         texture_format format = texture::from_bitmap_format(image.pixel_format());
         int32 width           = image.width();
         int32 height          = image.height();
         const uint8 *data     = image.data();
         if (!texture_.create(format, width, height, data)) {
            return on_error("could not create texture!");
         }

         image.destroy();
      }

      // note: load bitmap and create texture2
      {
         bitmap image;
         if (!image.create("assets/crate2.png")) {
            return on_error("could not load bitmap image!");
         }

         texture_format format = texture::from_bitmap_format(image.pixel_format());
         int32 width = image.width();
         int32 height = image.height();
         const uint8 *data = image.data();
         if (!texture2_.create(format, width, height, data)) {
            return on_error("could not create texture!");
         }

         image.destroy();
      }

      // note: create sampler state
      {
         if (!sampler_.create(SAMPLER_FILTER_MODE_LINEAR,
                             SAMPLER_ADDRESS_MODE_CLAMP,
                             SAMPLER_ADDRESS_MODE_CLAMP))
         {
            return on_error("could not create sampler state!");
         }
      }

      //// note: create index buffer
      //{
      //    const uint8 indices[] = {
      //        0, 1, 2,
      //        2, 3, 0,
      //    };
      //    if (!index_buffer_.create(sizeof(indices), indices)) {
      //        return on_error("could not create index buffer");
      //    }
      //}

      projection_ = glm::perspective(3.141592f * 0.25f, 16.0f / 9.0f, 1.0f, 100.0f);
      world_      = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -3.0f));
      world2_     = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-1.0f, -3.0f));
      world3_     = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, -3.0f));

      glm::mat4 projection = glm::perspective(3.141592f * 0.25f, 16.0f / 9.0f, 1.0f, 999.0f);
      camera_.set_projection(projection);

      return true;
   }

   void renderapp::on_exit()
   {
       skybox_.destroy();
   }

   bool renderapp::on_tick(const time &deltatime)
   {
       deltatime_ = static_cast<float>(deltatime.now().as_seconds()) / 2.0f;

      if (keyboard_.key_pressed(keyboard::key::escape)) {
         return false;
      }

      // note: lighting direction controller
      {
          if (keyboard_.key_down(keyboard::key::one))
          {
              lightdirection_ = glm::vec3{ 0.0f, 0.0f,-10.0f };
          }
          if (keyboard_.key_down(keyboard::key::two))
          {
              lightdirection_ = glm::vec3{ 0.0f, -10.0f,0.0f };
          }
          if (keyboard_.key_down(keyboard::key::three))
          {
              lightdirection_ = glm::vec3{ -10.0f, 0.0f,0.0f };
          }
          if (keyboard_.key_down(keyboard::key::four))
          {
              lightdirection_ = glm::vec3{ -10.0f, 5.0f,0.0f };
          }
      }

      // camera_.update();
      controller_.update(keyboard_, mouse_, deltatime);
      frustum_.construct(glm::transpose(camera_.projection_ * camera_.view_));

      const glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -3.0f));
      const glm::mat4 r = glm::rotate(glm::mat4(1.0f), time::now().as_seconds(), glm::vec3(0.0f, 1.0f, 0.0f));
      world_ = t * r;

      return true;
   }

   void renderapp::on_draw()
   {
      //renderer_.clear(0.1f, 0.3f, 0.4f, 1.0f);
      renderer_.clear(0.0f, 0.0f, 0.0f, 0.0f);

      renderer_.set_shader_program(shader_);
      renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_MATRIX, "u_projection", 1, glm::value_ptr(camera_.projection_));
      renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_MATRIX, "u_view", 1, glm::value_ptr(camera_.view_));
      renderer_.set_sampler_state(sampler_);
      renderer_.set_vertex_buffer(buffer_);
      renderer_.set_vertex_layout(layout_);

      renderer_.set_depth_state(true, true);
      renderer_.set_rasterizer_state(CULL_MODE_BACK);

      if (frustum_.is_inside(glm::vec3(world_[3]))) {
         renderer_.set_texture(texture_);
         renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_MATRIX, "u_world", 1, glm::value_ptr(world_));
         renderer_.draw(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, vertex_count_);
      }

      //if (frustum_.is_inside(glm::vec3(world2_[3]))) {
      //   renderer_.set_texture(texture2_);
      //   renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_MATRIX, "u_world", 1, glm::value_ptr(world2_));
      //   renderer_.draw(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, vertex_count_);
      //}

      if (frustum_.is_inside(glm::vec3(world3_[3]))) {
          renderer_.set_texture(texture2_);
          renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_MATRIX, "u_world", 1, glm::value_ptr(world3_));
          renderer_.draw(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, vertex_count_);
      }
  
      renderer_.set_shader_program(heightmap_shader_);
      renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "u_cameraposition", 1, glm::value_ptr(camera_.position_));
      renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_direction", 1, glm::value_ptr(lightdirection_));
      
      set_phong_reflection_uniforms(0, 2);
      //change_light();

      renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_MATRIX, "u_projection", 1, glm::value_ptr(camera_.projection_));
      renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_MATRIX, "u_view", 1, glm::value_ptr(camera_.view_));
      renderer_.set_rasterizer_state(CULL_MODE_BACK);   
      renderer_.set_vertex_buffer(vertex_buffer_);
      renderer_.set_vertex_layout(vertex_layout_);
      renderer_.set_index_buffer(index_buffer_);

      // check for wireframe mode.
      {
          if (keyboard_.key_down(keyboard::key::t))
          {
              glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          }
      }

      // note: render using the index buffer
      renderer_.draw_indexed(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,  
          INDEX_TYPE_UNSIGNED_INT,
          //INDEX_TYPE_UNSIGNED_BYTE,
          0,
          heightmap_index_count
      );

      skybox_.draw(renderer_, camera_);
   }
   
   void renderapp::set_phong_reflection_uniforms(int mode, int color)
   {
       // only default
       if (mode == 0)
       {
           if (color == 1)
           {
               material_shininess_float = 200;

               //renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_VEC3, "light_diffuse", 1, glm::value_ptr(glm::vec3{ 1,0.5f,1.0f }));
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_diffuse", 1, glm::value_ptr(glm::vec3{ 1,0.5f,1.0f }));

               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_ambient", 1, glm::value_ptr(glm::vec3{ 0.5f,0.5f,0.5f }));
               //renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_VEC3, "light_ambient", 1, glm::value_ptr(glm::vec3{ 1.0f,0.5f,1.0f }));

               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_specular", 1, glm::value_ptr(glm::vec3{ 1,0.5f,1.0f }));
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_FLOAT, "material_shininess", 1, material_shininess_pointer);
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_specular", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));
           }
           else if (color == 2)
           {
               material_shininess_float = 200;

               //renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_VEC3, "light_diffuse", 1, glm::value_ptr(glm::vec3{ 1,1,0.5f }));
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_diffuse", 1, glm::value_ptr(glm::vec3{ 1,1,0.5f }));

               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_ambient", 1, glm::value_ptr(glm::vec3{ 0.5f,0.5f,0.5f }));
               //renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_VEC3, "light_ambient", 1, glm::value_ptr(glm::vec3{ 1.0f,1.0f,0.5f }));

               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_specular", 1, glm::value_ptr(glm::vec3{ 1,1,0.5f }));
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_FLOAT, "material_shininess", 1, material_shininess_pointer);

               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_specular", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));

               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_diffuse", 1, glm::value_ptr(glm::vec3{0.5f, 0.5f, 0.0f}));
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_ambient", 1, glm::value_ptr(glm::vec3{0.3f, 0.2f, 0.0f}));
           }
           else if (color == 3)
           {
               //renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_VEC3, "light_diffuse", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_diffuse", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));

               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_ambient", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));
               //renderer_.set_shader_uniform(shader_, UNIFORM_TYPE_VEC3, "light_ambient", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));

               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "material_specular", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_FLOAT, "material_shininess", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));
               renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_specular", 1, glm::value_ptr(glm::vec3{ 1,1,1 }));
           }
       }
   }


   void renderapp::change_light()
   {

       renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_ambient", 1, glm::value_ptr(glm::vec3{ 0.5f,0.2f,0.1f }));


       // https://learnopengl.com/Lighting/Materials#:~:text=The%20diffuse%20material%20vector%20defines,a%20surface%2Dspecific%20color).
       glm::vec3 lightColor;
       lightColor.x = sin(deltatime_ * 2.0f);
       lightColor.y = sin(deltatime_ * 0.7f);
       lightColor.z = sin(deltatime_ * 1.3f)    ;


       glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
       glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

       renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_diffuse", 1, glm::value_ptr(diffuseColor));
       renderer_.set_shader_uniform(heightmap_shader_, UNIFORM_TYPE_VEC3, "light_specular", 1, glm::value_ptr(ambientColor));
   }
} // !avocado
