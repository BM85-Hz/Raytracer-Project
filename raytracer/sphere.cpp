//Student Name: Braxton Mott
//Student ID: 862208656

#include "sphere.h"
#include "ray.h"

Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    Hit hit = {0,-1,vec2 {}};

    vec3 oc = ray.endpoint - this->center;
    double a = dot(ray.direction, ray.direction);
    double b = 2 * dot(ray.direction, oc);
    double c = dot(oc, oc) - (this->radius * this->radius);
    double discriminante = b*b - 4*a*c;

    if(discriminante >= 0){
        double t_positive = (-b + sqrt(discriminante)) / (2*a);
        double t_negative = (-b - sqrt(discriminante)) / (2*a);
        if(t_positive > t_negative && t_negative >= small_t)
            hit.dist = t_negative;
        else
            hit.dist = t_positive;
    }

    Debug_Scope scope; 
    Pixel_Print("intersect test with ",this->name,"; hit: ",hit);

    return hit;
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal;
    vec3 rayPoint = ray.Point(hit.dist) - center; // compute the normal direction
    normal = rayPoint.normalized();
    return normal;
}

std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}