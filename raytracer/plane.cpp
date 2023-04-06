//Student Name: Braxton Mott
//Student ID: 862208656

#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the plane.  The plane's normal points outside.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    Hit hit = {0,-1,vec2 {}};

    double direction = dot(ray.direction, normal);
    double norm = 0.0;

    if(direction != 0){
        norm = dot((x- ray.endpoint), normal)/direction;
        if(norm >= small_t){
            hit.dist = norm;
        }
    }
    return hit;
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}
