#ifndef RENDER_SYSTEM_HPP_
#define RENDER_SYSTEM_HPP_

#include <entity/registry.hpp>
#include <entity/utility.hpp>
#include <glm/glm.hpp>

#include "ball_component.hpp"
#include "boundingboxes.hpp"
#include "collision.hpp"
#include "model_component.hpp"
#include "transform_component.hpp"
#include "light_component.hpp"
#include "animation_component.hpp"

// temp variable
bool render_wireframe = false;

void Render(entt::registry& registry) {
  auto view_model = registry.group<ModelComponent, TransformComponent>(entt::exclude<AnimationComponent>);

  for (auto& model : view_model) {
    auto& t = view_model.get<TransformComponent>(model);
    auto& m = view_model.get<ModelComponent>(model);

    glob::Submit(m.handle,
                 glm::translate(t.position) *
                     glm::rotate(-t.rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
                     glm::translate(-m.offset) * glm::scale(t.scale));
  }

  auto animated_models = registry.group<ModelComponent, TransformComponent, AnimationComponent>();
  for (auto& model : animated_models) {
	  auto& t = animated_models.get<TransformComponent>(model);
	  auto& m = animated_models.get<ModelComponent>(model);
	  auto& a = animated_models.get<AnimationComponent>(model);

	  glob::SubmitBAM(m.handle,
					  glm::translate(t.position) * 
						  glm::rotate(-t.rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
						  glm::translate(-m.offset) * glm::scale(t.scale),
							  a.bone_transforms);
  }

  //submit lights
  auto lights = registry.view<LightComponent, TransformComponent>();
  for (auto& l : lights) {
	  auto& transform = lights.get<TransformComponent>(l);
	  auto& light = lights.get<LightComponent>(l);

	  //glm::mat4 mat = glm::translate(transform.position) * glm::scale(transform.scale) * glm::mat4_cast(glm::quat(transform.rotation));
	  glm::vec3 pos = transform.position;
	  glm::vec3 dir = glm::quat(transform.rotation) * glm::vec3(1.f, 0.f, 0.f);
	  glob::SubmitLightSource(pos, light.color, light.radius, light.ambient);
  }


  // Render wireframes
  auto view_wireframe_obb = registry.view<physics::OBB, TransformComponent>();
  auto view_wireframe_sphere = registry.view<physics::Sphere>();
  auto view_wireframe_arena = registry.view<physics::Arena>();
  if (render_wireframe) {
    for (auto& w : view_wireframe_obb) {
      auto& obb = view_wireframe_obb.get<physics::OBB>(w);
      auto& transform = view_wireframe_obb.get<TransformComponent>(w);
      glob::SubmitCube(glm::translate(obb.center) *
                       glm::rotate(-transform.rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
                       glm::scale(glm::vec3(obb.extents[0],
                                                               obb.extents[1],
                                                               obb.extents[2])));
    }
    for (auto& w : view_wireframe_sphere) {
      auto& sphere = view_wireframe_sphere.get(w);
      glob::SubmitCube(glm::translate(sphere.center) * glm::scale(glm::vec3(sphere.radius)));
    }
    for (auto& w : view_wireframe_arena) {
      auto& arena = view_wireframe_arena.get(w);
      glob::SubmitCube(glm::scale(glm::vec3(arena.xmax-arena.xmin, arena.ymax-arena.ymin, arena.zmax-arena.zmin) * 0.5f));
    }
  }
}
#endif  // RENDER_SYSTEM_HPP_