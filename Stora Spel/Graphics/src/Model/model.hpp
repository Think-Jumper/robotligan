#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "mesh.hpp"

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <glm/gtx/transform.hpp>

#include "glob/Animation.hpp"
#include "glob/joint.hpp"
#include "glob/mesh_data.hpp"
#include "material/material.hpp"
#include "shader.hpp"

namespace glob {

class Model {
 private:
  Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transform);

  GLint TextureFromFile(const char* path, std::string directory,
                        aiTextureType type);

  void LoadModel(std::string path);
  void ProcessNode(aiNode* node, const aiScene* scene, glm::mat4 parent_transform = glm::identity<glm::mat4>());
  std::string PrintArmature(Joint parent, int depth);
  Joint* MakeArmature(aiNode* node);

  std::vector<Texture> texture_loaded_;
  std::vector<Mesh> mesh_;
  std::vector<Texture> LoadMaterielTextures(aiMaterial* material,
                                            aiTextureType type,
                                            std::string type_name,
                                            int tex_slot);

  void calcSortSphere();

  std::string filepath_;
  std::string directory_;

  bool is_loaded_ = false;
  bool humanoid_ = false;

  bool use_gl_ = true;
  bool is_emissive_ = false;
  bool is_transparent_ = false;
  bool is_glass_ = false;

  int num_diffuse_textures_ = 1;

  Material material_;
  float normal_map_scale_ = 1.f;
  float metallic_map_scale_ = 1.f;
  float roughness_map_scale_ = 1.f;

  float sort_sphere_radius = 0;
  glm::vec3 sort_sphere_center{0};
 public:
  Model();
  Model(const std::string& path);
  ~Model();

  void LoadFromFile(const std::string& path);
  bool IsLoaded() { return is_loaded_; };

  void Draw(ShaderProgram& shader);

  float MaxDistance(glm::mat4 transform, glm::vec3 point);

  int GetNumDiffuseTextures() { return num_diffuse_textures_; }

  std::vector<Joint*> bones_;
  std::vector<Animation*> animations_;
  glm::mat4 globalInverseTransform_;

  glob::MeshData GetMeshData();

  float TraceSortSphere(glm::vec3 cam_dir, glm::vec3 cam_pos, glm::mat4 transform);

  bool IsEmissive() { return is_emissive_; }
  void SetTransparent(bool is_transparent) { is_transparent_ = is_transparent; }
  bool IsTransparent() { return is_transparent_; }
  bool IsGlass() { return is_glass_; }
};

}  // namespace glob

#endif  // MODEL_HPP_
