#include <metal_stdlib>

using namespace metal;

struct Parameters {
    float2 resolution;
    float3 right;
    float3 up;
    float3 forward;
    float3 camera_position;
    uint triangle_count;
    uint max_hitpoint_count;
};

struct Material {
    float4 diffuse_color;
};

struct Hitpoint {
    float3 position;
    float3 normal;
    float3 ray_direction;
    uint2 pixel_location;
    uint material_index;
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

kernel void GenerateHitpoints(uint2                           gid            [[thread_position_in_grid]],
                              constant Parameters&            parameters     [[buffer(0)]],
                              constant Material*              materials      [[buffer(1)]],
                              constant float3*                vertices       [[buffer(2)]],
                              constant uint4*                 triangles      [[buffer(3)]],
                        
                              device atomic_uint&             hitpoint_count [[buffer(4)]],
                              device Hitpoint*                hitpoints      [[buffer(5)]]) {

    float2 normalized = float2(gid.xy) / (parameters.resolution - float2(1.0, 1.0)) - float2(0.5, 0.5);
    float3 view_direction = normalize(
                                      normalized.x * parameters.right +
                                      normalized.y * parameters.up +
                                      parameters.forward);
    
    Hitpoint hitpoint;
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
            hitpoint.pixel_location = gid;
            hitpoint.material_index = triangles[i][3];
            closest_hit = hit;
        }
    }
    
    if (closest_hit < 1.0 / 0.0) {
        uint hitpoint_index = atomic_fetch_add_explicit(&hitpoint_count, 1, memory_order_relaxed);
        if (hitpoint_index < parameters.max_hitpoint_count) {
            hitpoints[hitpoint_index] = hitpoint;
        }
    }
}

kernel void ClearTexture(uint2                           gid         [[thread_position_in_grid]],
                         texture2d<float, access::write> out_texture [[texture(0)]]) {
    if(gid.x < out_texture.get_width() && gid.y < out_texture.get_height()) {
        out_texture.write(float4(0.0, 0.0, 0.0, 0.0), gid);
    }
}

kernel void EvaluateRadiance(uint                            gid            [[thread_position_in_grid]],
                             constant Material*              materials      [[buffer(1)]],
                             constant uint&                  hitpoint_count [[buffer(4)]],
                             constant Hitpoint*              hitpoints      [[buffer(5)]],
                             
                             texture2d<float, access::write> out_texture    [[texture(0)]]) {
    if (gid >= hitpoint_count) {
        return;
    }
    constant const Material& material = materials[hitpoints[gid].material_index];
    out_texture.write(material.diffuse_color, hitpoints[gid].pixel_location);
}

//kernel void ShootViewRays(texture2d<float, access::write> out_texture [[texture(0)]],
//                          uint2                           gid         [[thread_position_in_grid]],
//                          constant Parameters&            parameters  [[buffer(0)]],
//                          constant Material*              materials   [[buffer(1)]],
//                          constant float3*                vertices    [[buffer(2)]],
//                          constant uint4*                 triangles   [[buffer(3)]],
//
//                          HitPoint* hitpoints [[buffer(4)]]) {
//    // Check if the pixel is within the bounds of the output texture
//    if((gid.x >= out_texture.get_width()) || (gid.y >= out_texture.get_height()))
//    {
//        // Return early if the pixel is out of bounds
//        return;
//    }
//
//
//
//    float2 normalized = float2(gid.xy) / (parameters.resolution - float2(1.0, 1.0)) - float2(0.5, 0.5);
//    float3 view_direction = normalize(
//                                    normalized.x * parameters.right +
//                                    normalized.y * parameters.up +
//                                    parameters.forward);
//
//    float4 pixel = float4(0.0);
//    float closest_hit = 1.0 / 0.0; // = infinity()
//    for (uint i = 0; i < parameters.triangle_count; ++i) {
//        float hit;
//        float3 barycentric_coordinates;
//        float3 triangle_normal;
//        if (IntersectTriangle(parameters.camera_position, view_direction,
//                              vertices[triangles[i][0]],
//                              vertices[triangles[i][1]],
//                              vertices[triangles[i][2]],
//                              hit, barycentric_coordinates, triangle_normal) &&
//            hit < closest_hit) {
//            closest_hit = hit;
//            pixel = materials[triangles[i][3]].diffuse_color;
//        }
//    }
//
//    out_texture.write(pixel, gid);
//}
