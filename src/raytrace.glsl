// based on https://antongerdelan.net/opengl/compute.html

#version 460 core
// set the workgroup size to 1x1 to only the compute shader on single pixels at a time
layout (local_size_x = 1, local_size_y = 1) in;

layout(std430, binding = 1) buffer primitiveBuffer
{
    int some_int;
    float fixed_array[42];
    float variable_array[];
};

struct material_s {
    float ka;	// ambient reflection coefficient
    float kd;	// diffuse coefficient
    float n;	// Phong specular reflection exponent
    float ks;	// material specular reflection coefficient
};

struct sphere_s {
    vec4 origin;
    vec4 diffuse_color;
    vec4 specular_color;
    material_s material;
    float radius;
    float padding[3]; // structs passed to the GPU via buffer objects need to be multiples of vec4 (128 bytes) so pad the sphere
};

struct light_s {
    vec4 origin;
    vec4 color;
    int type;
    float padding[3]; // structs passed to the GPU via buffer objects need to be multiples of vec4 (128 bytes) so pad the light
};

layout(std430, binding = 2) buffer sphereBuffer
{
   sphere_s spheres[];
};

layout(std430, binding = 3) buffer lightsBuffer
{
   light_s lights[];
};

layout(std430, binding = 4) buffer primaryRayBuffer
{
   vec4 primaryRays[];
};

// define the output as an image2d with rgba floats, with uniform buffer binding point 0
layout(rgba32f, binding = 0) uniform image2D img_output;

void main()
{
    // base pixel color for image
    vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
    // get the index in the global work group, which is the pixel x,y coordinate because workgroup size is 1x1
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    // trace against each sphere
    vec4 ray_origin = vec4(0.0, 0.0, 0.0, 1.0);
    uint index = pixel_coords.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x + pixel_coords.x;
    float min_depth = 99990.0f;
    for(int i = 0; i < spheres.length(); i++) {
        vec4 omc = ray_origin - spheres[i].origin;
        float b = dot(primaryRays[index], omc);
        float rsquared = spheres[i].radius * spheres[i].radius;
        float c = dot(omc, omc) - rsquared;
        float bsqmc = b * b - c;
        // hit one or the other side of the sphere
        if (bsqmc >= 0.0) {
            //pixel = vec4(0.4, 0.4, 1.0, 1.0);
            //pixel += spheres[i].diffuse_color;
            
            
            // calculate the ray-sphere intersection point
            
            vec4 s = spheres[i].origin - ray_origin;
            float tca = dot(s.xyz, primaryRays[index].xyz);
            float l2oc = dot(s.xyz,s.xyz);
            
            // if the ray originates outside of the sphere and points away, it doesn't intersect
            if((l2oc > rsquared) && (tca < 0)) {
                continue;
            }
            
            // determine the half cord distance squared. if this distance is less than 0, that indicates that the ray missed the sphere
	        float t2hc = rsquared - l2oc + tca * tca;
	        if(t2hc < 0) {
		        continue;
            }

            // FIXME: all of the vector normalization and dot products should be on the first 3 elements
            
            // find the intersection point
            float t = 0.0f;

            // if the ray originated within the sphere, t = tca +sqrt(t2hc)
            if (l2oc < rsquared) {
                t = tca + sqrt(t2hc);
            }
            // if the ray originated outside the sphere, t = tca - sqrt(t2hc)
            else if(l2oc > rsquared) {
                t = tca - sqrt(t2hc);
            }

            // depth buffer
            if(t < min_depth) {
                min_depth = t;
            } else {
                continue;
            }
            
            vec4 hit_point = ray_origin + primaryRays[index] * t;
            hit_point.w = 1.0f;
            vec4 v = normalize(ray_origin - hit_point);

            /*
            for(int j = 0; j < lights.length(); j++) {
                pixel += lights[j].color * spheres[i].diffuse_color;
            }
            */

            // Phong-Blinn shading
            
            // for each light source, contribute the color contribution
            for(int j = 0; j < lights.length(); j++) {
                
                vec4 l = normalize(lights[j].origin - hit_point);
                vec4 n = normalize(hit_point - spheres[i].origin);
                vec4 h = (v+l)/length(v+l);

                pixel += lights[j].color * (spheres[i].diffuse_color * spheres[i].material.kd * max(0, dot(n.xyz, l.xyz)) + spheres[i].material.ks * max(0, pow(dot(n.xyz, h.xyz), spheres[i].material.n)));

                //pixel = lights[j].color * dot(n.xyz, l.xyz);
                
            }
            
        }
    }

    // test shader contents here by varying color based on the x,y coordinate
    //pixel.r = gl_GlobalInvocationID.x/(1.0f * gl_NumWorkGroups.x * gl_WorkGroupSize.x);
    //pixel.g = 1.0f - pixel.r;
    //pixel.b = gl_GlobalInvocationID.y/(1.0f * gl_NumWorkGroups.y * gl_WorkGroupSize.y);

    // output to the pixel in the image
    imageStore(img_output, pixel_coords, pixel);
}