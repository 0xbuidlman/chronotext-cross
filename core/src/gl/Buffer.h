#pragma once

#include "gl/ShaderHelper.h"

namespace chr
{
  namespace gl
  {
    namespace buffer
    {
      enum TypeIndex : size_t
      {
        VERTEX,
        VERTEX_UV,
        VERTEX_RGBA,
        VERTEX_N,
        VERTEX_UV_RGBA,
        VERTEX_UV_N,
        VERTEX_RGBA_N,
        VERTEX_UV_RGBA_N,
        VEC2,
        VEC3,
        VEC4,
        GLUSHORT,
        GLUINT,
      };

      template<class T>
      struct TypeTraits
      {};

      template<class T>
      struct Element
      {
        std::vector<T> storage;
        size_t allocatedSize;
        int useCount;
        GLuint vboId;
      };
    }

    template<typename T>
    class Buffer
    {
    public:
      static constexpr size_t typeIndex = buffer::TypeTraits<T>::typeIndex;
      static constexpr GLenum target = buffer::TypeTraits<T>::target;

      int id;
      buffer::Element<T> *element;
      std::vector<T> &storage;
      GLenum usage;

      Buffer(GLenum usage = GL_DYNAMIC_DRAW)
      :
      id(buffer::TypeTraits<T>::usageCounter++),
      element(new buffer::Element<T>()),
      storage(element->storage),
      usage(usage)
      {
        element->useCount++;
      }

      Buffer(const Buffer &other)
      :
      id(other.id),
      element(other.element),
      storage(other.storage),
      usage(other.usage)
      {
        element->useCount++;
      }

      Buffer& operator=(const Buffer &other)
      {
        if (this != &other)
        {
          element = other.element;
          id = other.id;
          storage = other.storage;
          usage = other.usage;

          element->useCount++;
        }

        return *this;
      }

      ~Buffer()
      {
        element->useCount--;

        if (element->useCount == 0)
        {
          if (element->vboId != 0)
          {
            glDeleteBuffers(1, &element->vboId);
          }

          delete element;
        }
      }

      void clear()
      {
        storage.clear();
      }

      void reserve(size_t n)
      {
        storage.reserve(n);
      }

      size_t size() const
      {
        return storage.size();
      }

      bool empty() const
      {
        return storage.empty();
      }

      int useCount() const
      {
        return element->useCount;
      }

      void bind(const ShaderProgram &shader, bool forceUpload = false)
      {
        if (element->vboId == 0)
        {
          glGenBuffers(1, &element->vboId);
        }

        switch (typeIndex)
        {
          case buffer::GLUSHORT:
          case buffer::GLUINT:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element->vboId);
            break;

          default:
            glBindBuffer(target, element->vboId);
            ShaderHelper::bindAttributes<T>(shader);
            break;
        }

        if (forceUpload || uploadRequired)
        {
          upload();
          uploadRequired = false;
        }
      }

      void unbind(const ShaderProgram &shader)
      {
        switch (typeIndex)
        {
          case buffer::GLUSHORT:
          case buffer::GLUINT:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            break;

          default:
            ShaderHelper::unbindAttributes<T>(shader);
            glBindBuffer(target, 0);
            break;
        }
      }

      void draw(GLenum primitive)
      {
        switch (typeIndex)
        {
          case buffer::GLUSHORT:
          case buffer::GLUINT:
            glDrawElements(primitive, storage.size(), (typeIndex == buffer::GLUSHORT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
            break;

          default:
            glDrawArrays(primitive, 0, storage.size());
            break;
        }
      }

      void requestUpload()
      {
        uploadRequired = true;
      }

      template<typename... Args>
      inline void add(Args&&... args)
      {
        storage.emplace_back(std::forward<Args>(args)...);
      }

    protected:
      bool uploadRequired = true;

      void upload()
      {
        switch (usage)
        {
          case GL_STATIC_DRAW:
          {
            if (element->allocatedSize == 0)
            {
              element->allocatedSize = element->storage.size();
              glBufferData(target, element->storage.size() * sizeof(T), storage.data(), GL_STATIC_DRAW);
            }
          }
          break;

          case GL_DYNAMIC_DRAW:
          default:
          {
            if (element->allocatedSize < element->storage.size())
            {
              element->allocatedSize = element->storage.size();
              glBufferData(target, element->storage.size() * sizeof(T), nullptr, GL_DYNAMIC_DRAW);
            }

            glBufferSubData(target, 0, element->storage.size() * sizeof(T), storage.data());
          }
          break;
        }
      }
    };

    // ---

    namespace buffer
    {
      template<>
      struct TypeTraits<Vertex<XYZ>>
      {
        static constexpr size_t typeIndex = TypeIndex::VERTEX;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<Vertex<XYZ.UV>>
      {
        static constexpr size_t typeIndex = TypeIndex::VERTEX_UV;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<Vertex<XYZ.RGBA>>
      {
        static constexpr size_t typeIndex = TypeIndex::VERTEX_RGBA;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<Vertex<XYZ.N>>
      {
        static constexpr size_t typeIndex = TypeIndex::VERTEX_N;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<Vertex<XYZ.UV.RGBA>>
      {
        static constexpr size_t typeIndex = TypeIndex::VERTEX_UV_RGBA;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<Vertex<XYZ.UV.N>>
      {
        static constexpr size_t typeIndex = TypeIndex::VERTEX_UV_N;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<Vertex<XYZ.RGBA.N>>
      {
        static constexpr size_t typeIndex = TypeIndex::VERTEX_RGBA_N;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<Vertex<XYZ.UV.RGBA.N>>
      {
      public:
        static constexpr size_t typeIndex = TypeIndex::VERTEX_UV_RGBA_N;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<glm::vec2>
      {
        static constexpr size_t typeIndex = TypeIndex::VEC2;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<glm::vec3>
      {
        static constexpr size_t typeIndex = TypeIndex::VEC3;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<glm::vec4>
      {
        static constexpr size_t typeIndex = TypeIndex::VEC4;
        static constexpr GLenum target = GL_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<GLushort>
      {
        static constexpr size_t typeIndex = TypeIndex::GLUSHORT;
        static constexpr GLenum target = GL_ELEMENT_ARRAY_BUFFER;

        static int usageCounter;
      };

      template<>
      struct TypeTraits<GLuint>
      {
        static constexpr size_t typeIndex = TypeIndex::GLUINT;
        static constexpr GLenum target = GL_ELEMENT_ARRAY_BUFFER;

        static int usageCounter;
      };
    }
  }
}
