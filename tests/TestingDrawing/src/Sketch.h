#pragma once

#include "cross/Context.h"
#include "gl/Batch.h"
#include <gl/Matrix.h>
#include "gl/shaders/ColorShader.h"
#include "gl/shaders/TextureAlphaShader.h"

class Sketch : public chr::CrossSketch
{
public:
  Sketch();
  virtual ~Sketch() {}

  void setup() final;
  void draw() final;

  void initTextures();

protected:
  chr::gl::State state;

  chr::gl::Texture texture;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV.RGBA> textureBatch;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  chr::gl::IndexedVertexBatch<chr::gl::XYZ.RGBA> fillBatch;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ> strokeBatch;
  chr::gl::shaders::ColorShader colorShader;

  static void shapeToBatch(const std::vector<glm::vec2> &shape, chr::gl::IndexedVertexBatch<chr::gl::XYZ> &batch, chr::gl::Matrix &matrix);
  static std::vector<glm::vec2> starShape(float outerRadius);
};
