#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse* parse, std::istream& in)
{
    std::string file;
    in>>name>>file;
    Read_Obj(file.c_str());
}

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }

        if(sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        if(sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
            for(int i=0;i<3;i++) t[i]--;
            triangle_texture_index.push_back(t);
        }
    }
    num_parts=triangles.size();
}

// Compute the normal direction for the triangle with index part.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    Hit closest_hit = {std::numeric_limits<double>::max(),-1,vec2 {}};

    for (unsigned int i = 0; i < triangles.size(); i++) {
        Hit hit = Intersect_Triangle(ray, i);
        if (hit.dist < closest_hit.dist && hit.dist > small_t) {
            closest_hit = hit;
        }
    }
    if(closest_hit.dist == std::numeric_limits<double>::max())
        closest_hit.dist = -1;
    return closest_hit;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    const ivec3& t = triangles[hit.triangle];
    const vec3& A = vertices[t[0]];
    const vec3& B = vertices[t[1]];
    const vec3& C = vertices[t[2]];
    vec3 normal = (cross(B - C, A - C)).normalized();
    if (dot(normal, ray.direction) > 0) {
        normal = -normal;
    }
    return normal;
}


// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{//this is the triple product method as used in lecture 8 
    //and deduced as an answer on question 3 of the written portion
    const ivec3& t = triangles[tri];
    const vec3& A = vertices[t[0]];
    const vec3& B = vertices[t[1]];
    const vec3& C = vertices[t[2]];

    const vec3 CB = B - C;
    const vec3 CA = A - C;
    const vec3 ec = ray.endpoint - C;
    vec3 N = cross(CA, CB);

    double denom = dot(ray.direction, N);
    if (fabs(denom) < small_t) {
        // Ray is parallel to triangle plane
        return {-1, -1, {}};
    }
    double d = dot(A - ray.endpoint, N) / denom;// Position of intersection along ray
    if (d < small_t) {
        // Intersection point is behind ray origin
        return {-1, -1, {}};
    }

    // Barycentric coordinate weights used to check where inside the triangle the intersection is
    double alpha = dot(cross(ray.direction, CB), ec) / dot(cross(ray.direction, CB), CA);
    double beta = dot(cross(ray.direction, CA), ec) / dot(cross(ray.direction, CA), CB);
    double gamma = 1 - alpha - beta;

    if (alpha < -weight_tolerance || beta < -weight_tolerance || gamma < -weight_tolerance || alpha > 1.0 + weight_tolerance || beta > 1.0 + weight_tolerance || gamma > 1.0 + weight_tolerance) {
        // Intersection point is outside triangle
        return {-1, -1, {}};
    }

    // Added this portion in HW7 to return uv values for texture mapping
    vec2 uv;
    if (uvs.size() > 0){
        vec2 Auv = uvs[triangle_texture_index[tri] [0]];
        vec2 Buv = uvs[triangle_texture_index[tri] [1]];
        vec2 Cuv = uvs[triangle_texture_index[tri] [2]];
        uv = Auv * alpha + Buv * beta + Cuv * gamma;
    }

    return {d, tri, uv};
}


std::pair<Box,bool> Mesh::Bounding_Box(int part) const
{
    if(part<0)
    {
        Box box;
        box.Make_Empty();
        for(const auto& v:vertices)
            box.Include_Point(v);
        return {box,false};
    }

    ivec3 e=triangles[part];
    vec3 A=vertices[e[0]];
    Box b={A,A};
    b.Include_Point(vertices[e[1]]);
    b.Include_Point(vertices[e[2]]);
    return {b,false};
}
