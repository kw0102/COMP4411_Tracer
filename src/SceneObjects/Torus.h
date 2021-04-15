#ifndef __TORUS__
#define __TORUS__

#include "../scene/scene.h"

class Torus
	: public MaterialSceneObject
{
public:
	Torus(Scene* scene, Material* mat, double r1, double r2)
		: MaterialSceneObject(scene, mat), r1(r1), r2(r2) {}

	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
	virtual BoundingBox ComputeLocalBoundingBox() const
	{
		BoundingBox localbounds;
		localbounds.max = vec3f(-r1 - r2, -r1 - r2, -r2);
		localbounds.min = vec3f(r1 + r2, r1 + r2, r2);
		return localbounds;
	}

	double r1, r2;
};

#endif // !__TORUS__
