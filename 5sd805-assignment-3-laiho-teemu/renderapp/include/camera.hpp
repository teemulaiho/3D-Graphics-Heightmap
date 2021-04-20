// camera.hpp

#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

namespace avocado {
   struct frustum {
      enum class side {
         left,
         right,
         top,
         bottom,
         near,
         far,
         count,
      };

      frustum();

      void construct(const glm::mat4 &viewprojection);
      bool is_inside(const glm::vec3 &position) const;

      // Ax + By + Cz + D = 0
      glm::vec4 planes_[int(side::count)];
   };

   struct camera {
      camera();

      void update();
      void set_projection(const glm::mat4 &projection);
      void set_position(const glm::vec3 &position);

      void move_x(const float amount);
      void move_y(const float amount);
      void move_z(const float amount);
      void rotate_x(const float amount);
      void rotate_y(const float amount);

      float pitch_;
      float yaw_;
      glm::vec3 position_;
      glm::vec3 x_axis_;
      glm::vec3 y_axis_;
      glm::vec3 z_axis_;
      glm::mat4 projection_;
      glm::mat4 view_;
   };

   struct controller {
      controller(camera &camera);

      void update(const keyboard &kb, const mouse &m, const time &deltatime);
      void set_camera_speed(const float camera_speed);
      void set_mouse_invert_yaw(const bool invert);
      void set_mouse_invert_pitch(const bool invert);
      void set_mouse_sensitivity(const float mouse_sensitivity);

      camera &camera_;
      float camera_speed_;
      float mouse_yaw_;
      float mouse_pitch_;
      float mouse_sensitivity_;
      glm::vec2 previous_mouse_position_;
   };
} // !avocado

#endif // !CAMERA_HPP_INCLUDED
