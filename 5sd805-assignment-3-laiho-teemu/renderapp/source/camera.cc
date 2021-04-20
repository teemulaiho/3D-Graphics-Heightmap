// camera.cc

#include "main.hpp"
#include "camera.hpp"

namespace avocado {
   frustum::frustum()
   {
   }

   void frustum::construct(const glm::mat4 &projview)
   {
      // source: www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
      planes_[int(side::left)].x = projview[3][0] + projview[0][0];
      planes_[int(side::left)].y = projview[3][1] + projview[0][1];
      planes_[int(side::left)].z = projview[3][2] + projview[0][2];
      planes_[int(side::left)].w = projview[3][3] + projview[0][3];

      planes_[int(side::right)].x = projview[3][0] - projview[0][0];
      planes_[int(side::right)].y = projview[3][1] - projview[0][1];
      planes_[int(side::right)].z = projview[3][2] - projview[0][2];
      planes_[int(side::right)].w = projview[3][3] - projview[0][3];

      planes_[int(side::bottom)].x = projview[3][0] + projview[1][0];
      planes_[int(side::bottom)].y = projview[3][1] + projview[1][1];
      planes_[int(side::bottom)].z = projview[3][2] + projview[1][2];
      planes_[int(side::bottom)].w = projview[3][3] + projview[1][3];

      planes_[int(side::top)].x = projview[3][0] - projview[1][0];
      planes_[int(side::top)].y = projview[3][1] - projview[1][1];
      planes_[int(side::top)].z = projview[3][2] - projview[1][2];
      planes_[int(side::top)].w = projview[3][3] - projview[1][3];

      planes_[int(side::near)].x = projview[3][0] + projview[2][0];
      planes_[int(side::near)].y = projview[3][1] + projview[2][1];
      planes_[int(side::near)].z = projview[3][2] + projview[2][2];
      planes_[int(side::near)].w = projview[3][3] + projview[2][3];

      planes_[int(side::far)].x = projview[3][0] - projview[2][0];
      planes_[int(side::far)].y = projview[3][1] - projview[2][1];
      planes_[int(side::far)].z = projview[3][2] - projview[2][2];
      planes_[int(side::far)].w = projview[3][3] - projview[2][3];

      for (int i = 0; i < int(side::count); i++) {
         glm::vec3 normal = glm::vec3(planes_[i]);
         const float length = glm::length(normal);
         planes_[i].x = normal.x / length;
         planes_[i].y = normal.y / length;
         planes_[i].z = normal.z / length;
         planes_[i].w /= length;
      }
   }

   bool frustum::is_inside(const glm::vec3 &position) const
   {
      for (int index = 0; index < int(side::count); index++) {
         const glm::vec3 normal(planes_[index]);
         const float d = planes_[index].w;
         const float distance = glm::dot(normal, position) + d;     // add radius to distance (d)
         if (distance < 0.0f) {     // ALETERNATIVELY NEGATIVE RADIUS OF SPHERE
            return false;
         }
      }

      return true;
   }

   camera::camera()
      : pitch_(0.0f)
      , yaw_(0.0f)
      , position_(0.0f, 0.0f, 0.0f)
      , x_axis_(1.0f, 0.0f, 0.0f)
      , y_axis_(0.0f, 1.0f, 0.0f)
      , z_axis_(0.0f, 0.0f, 1.0f)
      , projection_(1.0f)
      , view_(1.0f)
   {
   }

   void camera::update()
   {
      glm::vec3 ax(1.0f, 0.0f, 0.0f);
      glm::vec3 ay(0.0f, 1.0f, 0.0f);
      glm::vec3 az(0.0f, 0.0f, 1.0f);

      glm::mat4 ry = glm::rotate(glm::mat4(1.0f), yaw_, ay);
      ax = glm::normalize(glm::mat3(ry) * ax);
      az = glm::normalize(glm::mat3(ry) * az);

      glm::mat4 rx = glm::rotate(glm::mat4(1.0f), pitch_, ax);
      ay = glm::normalize(glm::mat3(rx) * ay);
      az = glm::normalize(glm::mat3(rx) * az);

      view_[0][0] = ax.x;    view_[0][1] = ay.x;    view_[0][2] = az.x;
      view_[1][0] = ax.y;    view_[1][1] = ay.y;    view_[1][2] = az.y;
      view_[2][0] = ax.z;    view_[2][1] = ay.z;    view_[2][2] = az.z;
      view_[3][0] = -glm::dot(position_, ax);
      view_[3][1] = -glm::dot(position_, ay);
      view_[3][2] = -glm::dot(position_, az);

      x_axis_ = ax;
      y_axis_ = ay;
      z_axis_ = az;
   }

   void camera::set_projection(const glm::mat4 &projection)
   {
      projection_ = projection;
   }

   void camera::set_position(const glm::vec3 &position)
   {
      position_ = position;
   }

   void camera::move_x(const float amount)
   {
      position_ += x_axis_ * amount;
   }

   void camera::move_y(const float amount)
   {
      position_ += y_axis_ * amount;
   }

   void camera::move_z(const float amount)
   {
      position_ += z_axis_ * amount;
   }

   void camera::rotate_x(const float amount)
   {
      pitch_ += amount;
   }

   void camera::rotate_y(const float amount)
   {
      yaw_ += amount;
   }

   controller::controller(camera &camera)
      : camera_(camera)
      , camera_speed_(10.0f)
      , mouse_yaw_(0.022f)
      , mouse_pitch_(-0.022f)
      , mouse_sensitivity_(0.05f)
   {
   }

   void controller::update(const keyboard &kb, const mouse &m, const time &deltatime)
   {
      // note: keyboard
      if (kb.key_down(keyboard::key::w)) {
         camera_.move_z(-camera_speed_ * deltatime.as_seconds());
      }
      if (kb.key_down(keyboard::key::s)) {
         camera_.move_z(camera_speed_ * deltatime.as_seconds());
      }
      if (kb.key_down(keyboard::key::a)) {
         camera_.move_x(-camera_speed_ * deltatime.as_seconds());
      }
      if (kb.key_down(keyboard::key::d)) {
         camera_.move_x(camera_speed_ * deltatime.as_seconds());
      }
      if (kb.key_down(keyboard::key::q)) {
         camera_.move_y(-camera_speed_ * deltatime.as_seconds());
      }
      if (kb.key_down(keyboard::key::e)) {
         camera_.move_y(camera_speed_ * deltatime.as_seconds());
      }

      // note: mouse
      const glm::vec2 mouse_position(float(m.position().x_), float(m.position().y_));
      const glm::vec2 mouse_delta = mouse_position - previous_mouse_position_;
      if (m.button_down(mouse::button::left)) {
         camera_.rotate_x(-mouse_delta.y * mouse_sensitivity_ * mouse_pitch_);
         camera_.rotate_y(mouse_delta.x * mouse_sensitivity_ * mouse_yaw_);
      }

      previous_mouse_position_ = mouse_position;

      camera_.update();
   }

   void controller::set_camera_speed(const float camera_speed)
   {
      camera_speed_ = camera_speed;
   }

   void controller::set_mouse_invert_yaw(const bool invert)
   {
      mouse_yaw_ = invert ? -mouse_yaw_ : mouse_yaw_;
   }

   void controller::set_mouse_invert_pitch(const bool invert)
   {
      mouse_pitch_ = invert ? -mouse_pitch_ : mouse_pitch_;
   }

   void controller::set_mouse_sensitivity(const float mouse_sensitivity) 
   {
      mouse_sensitivity_ = mouse_sensitivity;
   }
} // !avocado
