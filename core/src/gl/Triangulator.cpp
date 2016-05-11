#include "gl/Triangulator.h"

using namespace std;
using namespace chr::math;

namespace chr
{
  namespace gl
  {
    static void* stdAlloc(void* userData, unsigned int size)
    {
      int* allocated = (int*)userData;
      *allocated += (int)size;
      return malloc(size);
    }

    static void stdFree(void* userData, void* ptr)
    {
      free(ptr);
    }

    Triangulator::Triangulator()
    {
      TESSalloc ma;
      memset(&ma, 0, sizeof(ma));
      ma.memalloc = stdAlloc;
      ma.memfree = stdFree;
      ma.userData = (void*)&allocated;
      ma.extraVertices = 256; // realloc not provided, allow 256 extra vertices.

      tess = tessNewTess(&ma);
    }

    Triangulator::~Triangulator()
    {
      tessDeleteTess(tess);
    }

    Triangulator& Triangulator::setWindingRule(int windingRule)
    {
      this->windingRule = windingRule;
      return *this;
    }

    Triangulator& Triangulator::setFrontFace(GLenum mode)
    {
      frontFace = mode;
      return *this;
    }

    Triangulator& Triangulator::setColor(const glm::vec4 &color)
    {
      this->color = color;
      return *this;
    }

    Triangulator& Triangulator::setColor(float r, float g, float b, float a)
    {
      color = { r, g, b, a };
      return *this;
    }

    template <>
    Triangulator& Triangulator::add<GL_CCW>(const Rectf &rect)
    {
      vector<glm::vec2> polygon;
      polygon.emplace_back(rect.x1y1());
      polygon.emplace_back(rect.x1y2());
      polygon.emplace_back(rect.x2y2());
      polygon.emplace_back(rect.x2y1());

      add(polygon);
      return *this;
    }

    template <>
    Triangulator& Triangulator::add<GL_CW>(const Rectf &rect)
    {
      vector<glm::vec2> polygon;
      polygon.emplace_back(rect.x1y1());
      polygon.emplace_back(rect.x2y1());
      polygon.emplace_back(rect.x2y2());
      polygon.emplace_back(rect.x1y2());

      add(polygon);
      return *this;
    }

    Triangulator& Triangulator::add(const vector<vector<glm::vec2>> &polygons)
    {
      for (auto &polygon : polygons)
      {
        add(polygon);
      }

      return *this;
    }

    Triangulator& Triangulator::add(const vector<glm::vec2> &polygon)
    {
      tessAddContour(tess, 2, polygon.data(), sizeof(glm::vec2), polygon.size());
      return *this;
    }

    template <>
    void Triangulator::stamp(IndexedVertexBatch<XYZ> &batch, Matrix &matrix)
    {
      performStamp(batch, matrix);
    }

    template <>
    void Triangulator::stamp(IndexedVertexBatch<XYZ.N> &batch, Matrix &matrix)
    {
      auto normal = matrix.transformNormal(0, 0, (frontFace == GL_CW) ? -1 : +1);
      performStamp(batch, matrix, normal);
    }

    template <>
    void Triangulator::stamp(IndexedVertexBatch<XYZ.RGBA> &batch, Matrix &matrix)
    {
      performStamp(batch, matrix, color);
    }

    template <>
    void Triangulator::stamp(IndexedVertexBatch<XYZ.N.RGBA> &batch, Matrix &matrix)
    {
      auto normal = matrix.transformNormal(0, 0, (frontFace == GL_CW) ? -1 : +1);
      performStamp(batch, matrix, normal, color);
    }

    template <>
    void Triangulator::extrude(IndexedVertexBatch<XYZ> &batch, Matrix &matrix, float distance)
    {
      if (distance != 0)
      {
        performExtrude(batch, matrix, distance);
      }
    }

    template <>
    void Triangulator::extrude(IndexedVertexBatch<XYZ.N> &batch, Matrix &matrix, float distance)
    {
      if (distance != 0)
      {
        performExtrudeWithNormals(batch, matrix, distance);
      }
    }

    template <>
    void Triangulator::extrude(IndexedVertexBatch<XYZ.RGBA> &batch, Matrix &matrix, float distance)
    {
      if (distance != 0)
      {
        performExtrude(batch, matrix, distance, color);
      }
    }

    template <>
    void Triangulator::extrude(IndexedVertexBatch<XYZ.N.RGBA> &batch, Matrix &matrix, float distance)
    {
      if (distance != 0)
      {
        performExtrudeWithNormals(batch, matrix, distance, color);
      }
    }
  }
}