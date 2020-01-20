#pragma once
#include "math3d.h"

// tools for finding intersections
struct Ray
{
	vec3d	origin;			// origin of ray
	vec3d	direction;		// direction of ray (must be unit vector!)
};

//-----------------------------------------------------------------------------
struct Intersection
{
	vec3d	point;			// point of intersection
	float	r[2];			// natural coordinates
	int		m_index;		// index of item that was intersected (context dependent)
	int		m_faceIndex;	// index of face that was intersected (context dependent)
};

//-----------------------------------------------------------------------------
struct Triangle
{
	vec3d	r0;
	vec3d	r1;
	vec3d	r2;
	vec3d	fn;	// face normal
};

//-----------------------------------------------------------------------------
struct Quad
{
	vec3d	r0;
	vec3d	r1;
	vec3d	r2;
	vec3d	r3;
};

//-----------------------------------------------------------------------------
// Find intersection of a ray with a triangle
// To evaluate the normal automatically, set evalNormal to true. Otherwise, the normal in Triangle is used
bool IntersectTriangle(const Ray& ray, const Triangle& tri, Intersection& q, bool evalNormal = true);

//-----------------------------------------------------------------------------
// Find intersection of a ray with a quad
bool IntersectQuad(const Ray& ray, const Quad& quad, Intersection& q);
bool FastIntersectQuad(const Ray& ray, const Quad& quad, Intersection& q);
