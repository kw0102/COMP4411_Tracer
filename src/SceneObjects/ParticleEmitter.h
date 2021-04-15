#ifndef __PARTICLE_EMITTER_
#define __PARTICLE_EMITTER_

#include "../scene/scene.h"
#include <random>
#include <vector>

struct Particle
{
	vec3f origin;
	vec3f p;
	vec3f v;
	vec3f color;
	vec3f startColor;
	vec3f endColor;
	double life;
	double maxLife;
};

class ParticleEmitter : public MaterialSceneObject
{
public:
	ParticleEmitter(Scene* scene, Material* mat,
		int numEmit, int maxNumParticles,
		double minLife, double maxLife,
		double minSpeed, double maxSpeed,
		vec3f startColorMin, vec3f startColorMax,
		vec3f endColorMin, vec3f endColorMax,
		vec3f force)
		: MaterialSceneObject(scene, mat),
		  numEmit(numEmit), maxNumParticles(maxNumParticles),
		  minLife(minLife), maxLife(maxLife),
		  minSpeed(minSpeed), maxSpeed(maxSpeed),
		  startColorMin(startColorMin), startColorMax(startColorMax),
		  endColorMin(endColorMin), endColorMax(endColorMax),
		  force(force)
	      {}

	virtual bool hasBoundingBoxCapability() const { return false; }
	virtual bool intersectLocal(const ray& r, isect& i) const;
	void init();

	double frand(double min, double max)
	{
		return min + ((double)rand() / RAND_MAX) * (max - min);
	};

	vector<Particle> particles;
	int numEmit, maxNumParticles;
	double minLife, maxLife, minSpeed, maxSpeed;
	vec3f startColorMin, startColorMax, endColorMin, endColorMax;
	vec3f force;
	int iteration = 500;
	double size = 0.5;

};

#endif // !__PARTICLE_EMITTER_
