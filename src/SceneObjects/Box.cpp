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
bool Box::getLocalUV(const ray& r, const isect& i, double& u, double& v) const
{
	vec3f pos = transform->globalToLocalCoords(r.getPosition());
	vec3f dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	double length = dir.length();
	dir /= length;

	ray localRay(pos, dir);
	isect icopy = i;
	if (intersectLocal(localRay, icopy)) {
		vec3f localIscePoint = localRay.at(icopy.t);
		float x = localIscePoint[0];
		float y = localIscePoint[1];
		float z = localIscePoint[2];

		float absX = fabs(x);
		float absY = fabs(y);
		float absZ = fabs(z);

		int isXPositive = x > 0 ? 1 : 0;
		int isYPositive = y > 0 ? 1 : 0;
		int isZPositive = z > 0 ? 1 : 0;

		float maxAxis, uc, vc;

		// POSITIVE X
		if (isXPositive && absX >= absY && absX >= absZ) {
			// u (0 to 1) goes from +z to -z
			// v (0 to 1) goes from -y to +y
			maxAxis = absX;
			uc = -z;
			vc = y;
			//*index = 0;
		}
		// NEGATIVE X
		if (!isXPositive && absX >= absY && absX >= absZ) {
			// u (0 to 1) goes from -z to +z
			// v (0 to 1) goes from -y to +y
			maxAxis = absX;
			uc = z;
			vc = y;
			//*index = 1;
		}
		// POSITIVE Y
		if (isYPositive && absY >= absX && absY >= absZ) {
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from +z to -z
			maxAxis = absY;
			uc = x;
			vc = -z;
			//*index = 2;
		}
		// NEGATIVE Y
		if (!isYPositive && absY >= absX && absY >= absZ) {
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from -z to +z
			maxAxis = absY;
			uc = x;
			vc = z;
			//*index = 3;
		}
		// POSITIVE Z
		if (isZPositive && absZ >= absX && absZ >= absY) {
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from -y to +y
			maxAxis = absZ;
			uc = x;
			vc = y;
			//*index = 4;
		}
		// NEGATIVE Z
		if (!isZPositive && absZ >= absX && absZ >= absY) {
			// u (0 to 1) goes from +x to -x
			// v (0 to 1) goes from -y to +y
			maxAxis = absZ;
			uc = -x;
			vc = y;
			//*index = 5;
		}

		// Convert range from -1 to 1 to 0 to 1
		u = 0.5f * (uc / maxAxis + 1.0f);
		v = 0.5f * (vc / maxAxis + 1.0f);
	
		
		return true;
	}
	else {
		return false;
	}

}
