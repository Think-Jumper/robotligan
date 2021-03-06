
#include <iostream>

#include <msdfgen/core/Bitmap.h>
#include <msdfgen/core/save-bmp.h>

#include <msdfgen/ext/import-font.h>

#include <glm/gtx/transform.hpp>

#include <direct.h>
#include <stdlib.h>
#include <lodepng.hpp>

#include "Font2D.hpp"
#include "glob/window.hpp"

bool FileAlreadyExists(const std::string& path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

void glob::Font2D::GenerateMsdfShapes(const std::string& font_path) {
  msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
  if (ft) {
    font = loadFont(ft, font_path.c_str());
    if (font) {
      msdfgen::Shape shape;
      FT_Face face = font->face;

      FT_Set_Char_Size(face, 32, 32, 1280, 720);

      unsigned int end_char = 255;
      unsigned int start_char = 0;
      unsigned int num_chars = end_char - start_char;

      for (int i = 0; i < num_chars; i++) {
        if (loadGlyph(shape, font, start_char + i)) {
          shapes.push_back(shape);
          if (FT_Error Error =
                  FT_Load_Char(face, (unsigned)i, FT_LOAD_RENDER)) {
            std::cout << "Failed to load Glyph: " << i << " Error: " << Error
                      << "\n";
            advances_.push_back(0);
          } else {
            advances_.push_back(face->glyph->advance.x >> 6);
          }
        }
      }
    }
  }
}

void glob::Font2D::GenerateMsdfFont(const std::string& font_path,
                                    const std::string& output_path) {
  msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
  if (ft) {
    font = loadFont(ft, font_path.c_str());
    if (font) {
      msdfgen::Shape shape;

      unsigned int end_char = 256;
      unsigned int start_char = 0;
      unsigned int num_chars = end_char - start_char;

      unsigned int v_size =
          (unsigned int)4 * width_ * height_ * side_dim_ * side_dim_;
      std::vector<unsigned char> pixels;
      pixels.resize(v_size);
      // pixels.assign(v_size, 0);

      double fscale = 0;
      msdfgen::getFontScale(fscale, font);

      double kerning = 0;
      msdfgen::getKerning(kerning, font, (int)'R', (int)'I');

      for (int i = 0; i < num_chars; i++) {
        if (loadGlyph(shape, font, start_char + i)) {
          // shape.normalize();
          shapes.push_back(shape);

          FT_Face face = font->face;

          FT_Set_Char_Size(face, width_, height_, 1280, 720);

          if (FT_Error Error = FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            std::cout << "Failed to load Glyph: " << i << " Error: " << Error
                      << "\n";
            advances_.push_back(0);
          } else {
            advances_.push_back(face->glyph->advance.x >> 6);
          }

          double l = 0, r = 0, t = 0, b = 0;
          shape.bounds(l, b, r, t);
          // std::cout << "Char: " << (char)i << " " << l << ", " << r << "\n";

          // shape.normalize();
          //                      max. angle
          edgeColoringSimple(shape, 3.0, 0ULL);

          //           image width, height
          msdfgen::Bitmap<float, 3> msdf(height_, width_);
          //                     range, scale, translation

          double range = 4.0;
          // 16 / fscale
          double scale = (width_/32.0)*28.0 / fscale;
          auto translate = msdfgen::Vector2(2.0, 8.0);
          double edge_threshold = 1.09;
          bool overlap_support = false;
          msdfgen::generateMSDF(msdf, shape, range, scale, translate,
                                edge_threshold, overlap_support);

          // msdfgen::msdfErrorCorrection(msdf, msdfgen::Vector2(0.0, 0.0));

          int sum = 0;
          for (unsigned int yy = 0; yy < height_; yy++) {
            for (unsigned int xx = 0; xx < width_; xx++) {
              char r = msdfgen::pixelFloatToByte(msdf(xx, yy)[0]);
              char g = msdfgen::pixelFloatToByte(msdf(xx, yy)[1]);
              char b = msdfgen::pixelFloatToByte(msdf(xx, yy)[2]);
              sum += (int)*reinterpret_cast<unsigned char*>(&r);
              sum += (int)*reinterpret_cast<unsigned char*>(&g);
              sum += (int)*reinterpret_cast<unsigned char*>(&b);
            }
          }
          float max_sum = 255 * 3 * height_ * width_;

          // :^)
          bool inverted = (sum / max_sum) > 0.6f;

          for (unsigned int yy = 0; yy < height_; yy++) {
            for (unsigned int xx = 0; xx < width_; xx++) {
              char r = msdfgen::pixelFloatToByte(msdf(xx, yy)[0]);
              char g = msdfgen::pixelFloatToByte(msdf(xx, yy)[1]);
              char b = msdfgen::pixelFloatToByte(msdf(xx, yy)[2]);

              if (inverted) {
                r = 255 - (unsigned char)r;
                g = 255 - (unsigned char)g;
                b = 255 - (unsigned char)b;
              }

              int jumps_down = i / side_dim_;
              int jumps_right = i % side_dim_;

              unsigned int pos =
                  (side_dim_ * width_ - (jumps_down * width_ + yy) - 1) *
                      side_dim_ * width_ * 4 +
                  (jumps_right * height_ + xx) * 4;

              pixels[pos + 0] = r;
              pixels[pos + 1] = g;
              pixels[pos + 2] = b;
              pixels[pos + 3] = 255;
            }
          }
        }
      }

      if (lodepng::encode(output_path, pixels, width_ * side_dim_,
                          height_ * side_dim_, LCT_RGBA)) {
        std::cout << "[ERROR] Could not generate sprite-sheet for font\n";
      }
      font_texture_ = pixels;
      texture_width_ = width_ * side_dim_;
      texture_height_ = height_ * side_dim_;

      // destroyFont(font);
    }
    // deinitializeFreetype(ft);
  }
}

std::string glob::Font2D::GenerateFontDirectoryPath(const std::string* path) {
  std::string p = "assets/fonts/generated";

  int start_of_name = path->find_last_of("/");
  int end_of_name = path->find_last_of(".");
  int len = end_of_name - start_of_name;
  std::string only_name = path->substr(start_of_name, len);

  std::string font_folder_name = p + only_name + "_msdf.png";

  return font_folder_name;
}

void glob::Font2D::CreateTexture() {
  GLuint texture_id;
  glGenTextures(1, &texture_id);

  // Load texture

  // Generate texture data
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width_, texture_height_, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, font_texture_.data());
  // glGenerateMipmap(GL_TEXTURE_2D);

  // Set some parameters for the texture

  glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture

  font_texture_.clear();
  font_texture_.reserve(1);
  font_texture_.shrink_to_fit();

  tex_id = texture_id;
}

glob::Font2D::Font2D() {}

glob::Font2D::Font2D(const std::string& path) { LoadFromFile(path); }

glob::Font2D::~Font2D() {}

bool glob::Font2D::LoadFromFile(const std::string& path) {
  if (!FileAlreadyExists(path)) {
    std::cout << "[ERROR] font2D.cpp: Could not find font: " << path << "\n";
  }

  std::string font_folder_name = GenerateFontDirectoryPath(&path);
  if (!FileAlreadyExists(font_folder_name)) {
    GenerateMsdfFont(path, font_folder_name);
  } else {
    unsigned error = lodepng::decode(font_texture_, texture_width_,
                                     texture_height_, font_folder_name);
    if (error != 0) {
      std::cout << "ERROR: Could not load texture: " << font_folder_name
                << "\n";
      return false;
    }
    GenerateMsdfShapes(path);
  }
  CreateTexture();
  is_loaded_ = true;
  directory_ = font_folder_name;
  return true;
}

void glob::Font2D::Draw(ShaderProgram& shader, glm::vec2 pos, unsigned int size,
                        std::string text, glm::vec4 color, bool visible,
                        bool equal_spacing, float spacing) {
  const char* chars = text.c_str();
  unsigned int len = text.length();

  //////////////////////////////////
  // for backwards compatibility
  size *= 16. / 28.;
  pos.x -= 0;
  pos.y -= size*10.f/32.f;
  //////////////////////////////////

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  shader.uniform("pxRange", 1.4f * float(width_)/32.f);
  shader.uniform("bgColor", glm::vec4(0, 0, 0, 0));
  shader.uniform("fgColor", color);
  shader.uniform("msdf", 0);
  shader.uniform("screen_dims", glob::window::GetWindowDimensions());
  // shader.uniform("t_pos", pos);
  shader.uniform("size", size);

  double offset_accum = 0;
  for (int i = 0; i < len; i++) {
    unsigned char cur = *(unsigned char*)(chars + i);

    // FT_Load_Char(face, (FT_ULong)cur, FT_LOAD_RENDER);
    // double kerning = 0;
    // if (i + 1 < len) msdfgen::getKerning(kerning, font, cur, chars[i + 1]);

    // shader.uniform("character", (GLint)cur);
    shader.uniform("character", cur);
    shader.uniform("t_pos", pos + glm::vec2(offset_accum, 0));
    if (visible) {
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    // std::cout << "Char: " << (char)cur << " " << l << ", " << r << "\n";

    // double kerning = 0;
    // if (i + 1 < len) msdfgen::getKerning(kerning, font, (int)cur,
    // (int)chars[i + 1]);

    offset_accum += GetAdvance(cur, size, equal_spacing, spacing);
    // std::cout << offset_accum << "\n";
  }
  // std::cout << "\n";

  glBindTexture(GL_TEXTURE_2D, 0);
}

double glob::Font2D::GetAdvance(char c, float size, bool equal_spacing,
                                double spacing) {
  double result = 0;
  if (c == ' ') {
    result = size / 3;
  } else {
    double r = 0;
    r = advances_[(unsigned char)c];
    if (!equal_spacing)
      result = r * .03 * (28. / 16.) * double(size);
    else
      result = spacing;
  }
  return result;
}

void glob::Font2D::Draw3D(ShaderProgram& shader, glm::vec3 center, float size,
                          std::string text, glm::vec4 color,
                          glm::mat4 rotation) {
  const char* chars = text.c_str();
  unsigned int len = (unsigned int)text.length();

  //////////////////////////////////
  // for backwards compatibility
  size *= 16.f / 28.f;
  //////////////////////////////////

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  shader.uniform("pxRange", 1.4f* float(width_)/32.f);
  shader.uniform("fgColor", color);
  shader.uniform("msdf", 0);
  shader.uniform("size", size);
  shader.uniform("center", center);

  shader.uniform("rotation", rotation);
  glm::vec3 dir = glm::vec3(1.f, 0.f, 0.f);

  double total_accum = 0.0;
  for (unsigned int i = 0; i < len; i++) {
    unsigned char cur = *(unsigned char*)(chars + i);

    total_accum += 2.f*GetAdvance(cur, size);
    // std::cout << offset_accum << "\n";
  }
  total_accum = total_accum - (total_accum / len);
  double offset_accum = -total_accum / 2.0;
  for (unsigned int i = 0; i < len; i++) {
    unsigned char cur = *(unsigned char*)(chars + i);

    // FT_Load_Char(face, (FT_ULong)cur, FT_LOAD_RENDER);
    // double kerning = 0;
    // if (i + 1 < len) msdfgen::getKerning(kerning, font, cur, chars[i + 1]);

    // shader.uniform("character", (GLint)cur);
    shader.uniform("character", cur);
    shader.uniform("offset", dir * (float)offset_accum);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // std::cout << "Char: " << (char)cur << " " << l << ", " << r << "\n";

    // double kerning = 0;
    // if (i + 1 < len) msdfgen::getKerning(kerning, font, (int)cur,
    // (int)chars[i + 1]);

    offset_accum += 2.f*GetAdvance(*(chars + i), size);
    // std::cout << offset_accum << "\n";
  }
  // std::cout << "\n";

  glBindTexture(GL_TEXTURE_2D, 0);
}