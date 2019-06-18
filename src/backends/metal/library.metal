#include <metal_stdlib>

using namespace metal;

constant float3 kLightDirection(0, 0, -1);

struct InVertex
{
    float4 position [[attribute(0)]];
    float4 normal [[attribute(1)]];
    float2 texCoords [[attribute(2)]];
};

struct ProjectedVertex
{
    float4 position [[position]];
    float3 normal [[user(normal)]];
    float2 texCoords [[user(tex_coords)]];
};

struct Uniforms
{
    float4x4 modelMatrix;
    float3x3 normalMatrix;
    float4x4 modelViewProjectionMatrix;
};

vertex ProjectedVertex vertex_project(InVertex inVertex [[stage_in]],
                                      constant Uniforms &uniforms [[buffer(1)]])
{
    ProjectedVertex outVert;
    outVert.position = uniforms.modelViewProjectionMatrix * float4(inVertex.position);
    outVert.normal = uniforms.normalMatrix * float4(inVertex.normal).xyz;
    outVert.texCoords = inVertex.texCoords;
    return outVert;
}

fragment half4 fragment_texture(ProjectedVertex vert [[stage_in]],
                                texture2d<float, access::sample> texture [[texture(0)]],
                                sampler texSampler [[sampler(0)]])
{
    float diffuseIntensity = max(0.33, dot(normalize(vert.normal), -kLightDirection));
    float4 diffuseColor = texture.sample(texSampler, vert.texCoords);
    float4 color = diffuseColor * diffuseIntensity;
    return half4(color.r, color.g, color.b, 1);
}

struct Parameters {
    float2 resolution;
    float3 right;
    float3 up;
    float3 forward;
    float3 camera_position;
    uint triangle_count;
};

bool IntersectTriangle(float3 ray_origin, float3 ray_direction, float3 p0, float3 p1, float3 p2,
                       thread float& hit, thread float3& barycentricCoord, thread float3& triangleNormal)
{
    const float3 e0 = p1 - p0;
    const float3 e1 = p0 - p2;
    triangleNormal = cross( e1, e0 );
    
    const float3 e2 = ( 1.0 / dot( triangleNormal, ray_direction ) ) * ( p0 - ray_origin );
    const float3 i  = cross( ray_direction, e2 );
    
    barycentricCoord.y = dot( i, e1 );
    barycentricCoord.z = dot( i, e0 );
    barycentricCoord.x = 1.0 - (barycentricCoord.z + barycentricCoord.y);
    hit   = dot( triangleNormal, e2 );
    
    return  /*(hit < ray.tmax) && */ (hit > 0.0001) && all(barycentricCoord >= float3(0.0));
}

kernel void ShootViewRays(texture2d<float, access::write> out_texture [[texture(0)]],
                          uint2                           gid         [[thread_position_in_grid]],
                          constant Parameters&            parameters  [[buffer(0)]],
                          constant float4*                materials   [[buffer(1)]],
                          constant float3*                vertices    [[buffer(2)]],
                          constant uint4*                 triangles   [[buffer(3)]]) {
    // Check if the pixel is within the bounds of the output texture
    if((gid.x >= out_texture.get_width()) || (gid.y >= out_texture.get_height()))
    {
        // Return early if the pixel is out of bounds
        return;
    }
    
    
    
    float2 normalized = float2(gid.xy) / (parameters.resolution - float2(1.0, 1.0)) - float2(0.5, 0.5);
    float3 view_direction = normalize(
                                    normalized.x * parameters.right +
                                    normalized.y * parameters.up +
                                    parameters.forward);
    
    float4 pixel = float4(0.0);
    float closest_hit = 1.0 / 0.0; // = infinity()
    for (uint i = 0; i < parameters.triangle_count; ++i) {
        float hit;
        float3 barycentric_coordinates;
        float3 triangle_normal;
        if (IntersectTriangle(parameters.camera_position, view_direction,
                              vertices[triangles[i][0]],
                              vertices[triangles[i][1]],
                              vertices[triangles[i][2]],
                              hit, barycentric_coordinates, triangle_normal) &&
            hit < closest_hit) {
            closest_hit = hit;
            pixel = materials[triangles[i][3]];
        }
    }
    
    out_texture.write(pixel, gid);
}
