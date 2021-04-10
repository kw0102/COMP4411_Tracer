#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.

	BoundingBox b = ComputeLocalBoundingBox();

	double tMin, tMax;
	if (!b.intersect(r, tMin, tMax))
		return false;

	if (tMin < RAY_EPSILON || tMax < RAY_EPSILON || abs(tMax - tMin) < RAY_EPSILON)
		return false;

	i.t = tMin;
	vec3f P = r.at(i.t);
	i.N = vec3f(0, 0, 0);

	if (abs(P[0] - 0.5) < RAY_EPSILON)
		i.N = vec3f(1, 0, 0);
	else if (abs(P[0] + 0.5) < RAY_EPSILON)
		i.N = vec3f(-1, 0, 0);
	else if (abs(P[1] - 0.5) < RAY_EPSILON)
		i.N = vec3f(0, 1, 0);
	else if (abs(P[1] + 0.5) < RAY_EPSILON)
		i.N = vec3f(0, -1, 0);
	else if (abs(P[2] - 0.5) < RAY_EPSILON)
		i.N = vec3f(0, 0, 1);
	else if (abs(P[2] + 0.5) < RAY_EPSILON)
		i.N = vec3f(0, 0, -1);

	i.obj = this;

	return true;
}
