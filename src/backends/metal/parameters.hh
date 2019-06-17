#include <simd/simd.h>

namespace rainbow {

struct Parameters {
  vector_float2 resolution;
  vector_float3 right;
  vector_float3 up;
  vector_float3 forward;
};

}  // namespace rainbow
