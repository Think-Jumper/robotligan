#ifndef SSAO_HPP_
#define SSAO_HPP_

#include <glad\glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "blur.hpp"
#include "postprocess.hpp"  
#include "glob/camera.hpp"

namespace glob {
class Ssao {
 public:
  Ssao();
  ~Ssao();
  void Init(Blur& blur);

  void Process(PostProcess& post_process, Blur& blur, Camera camera);

  void BindSsaoTexture(int slot);

 private:
  std::vector<glm::vec3> noise_;
  std::vector<glm::vec3> kernel_;

  GLuint noise_texture_;
  GLuint ssao_texture_;
  GLuint blurred_ssao_texture_;
  GLuint internal_format_ = 0;

  // set in constructor
  float downscale_ = 1.f;
  int num_samples_ = 0;

  uint64_t blur_id_;

  ShaderProgram shader;

  void BindNoiseTexture(int slot);
  void CreateKernelAndNoise();
  void GenerateTextures();
};

}  // namespace glob

#endif  // !SSAO_HPP_
