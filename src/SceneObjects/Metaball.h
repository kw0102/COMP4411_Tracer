#ifndef __METABALL__
#define __METABALL__

#include "../scene/scene.h"
#include <vector>

struct Ball {
	vec3f pos;
	double str;
};

struct Intersection {
	double t;
	bool isEntry;
	Ball ball;
};

class Metaball : public MaterialSceneObject
{
public:
	Metaball(Scene* scene, Material* mat)
		: MaterialSceneObject(scene, mat) 
	{
		add({ {0,0,-0.5}, 1 });
		add({ {0,1,0}, 1 });
		add({ {0,-1,0}, 1 });
		add({ {0,2,-0.5}, 1 });
		add({ {0,-2,-0.5}, 1 });
	}

	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
	virtual BoundingBox ComputeLocalBoundingBox() const
	{
		BoundingBox localbounds;
		localbounds.max = vec3f(0, 0, 0);
		localbounds.min = vec3f(0, 0, 0);

		if (!balls.empty())
		{
			const Ball& b = balls.at(0);
			localbounds.max = vec3f(b.pos[0] + b.str, b.pos[1] + b.str, b.pos[2] + b.str);
			localbounds.min = vec3f(b.pos[0] - b.str, b.pos[1] - b.str, b.pos[2] - b.str);
		}

		for (Ball b : balls)
		{
			for (int i = 0; i < 3; i++)
			{
				if (b.pos[i] + b.str > localbounds.max[i])
					localbounds.max[i] = b.pos[i] + b.str;
				if (b.pos[i] - b.str > localbounds.min[i])
					localbounds.min[i] = b.pos[i] - b.str;
			}
		}

		return localbounds;
	}

	void add(Ball ball);
	bool intersectBoundingSphere(const Ball& ball, const ray& r, double& tMin, double& tMax) const;
	double totalStrength(vector<Ball> balls, vec3f p) const;


	std::vector<Ball> balls;
	double threshold = 0.2;
};

#endif // !__METABALL__
