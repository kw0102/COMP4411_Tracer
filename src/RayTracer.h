#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

#include <stack>

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth, std::stack<Material> materials );


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );
	void loadHeightField(unsigned char* pHeightMap, unsigned char* pColorMap, const int w, const int h);

	bool sceneLoaded();

	// Recursion depth
	int maxDepth = 0;

	// Anti-aliasing
	bool useSuperSampling = false;
	bool useJitter = false;
	int numSuperSample = 1;
	bool useAdaptiveSampling = false;

	// Effects
	bool depthOfField = false;
	double focalLength = 2.0f;
	double aperture = 1.0f;
	bool motionBlur = false;
	bool softShadow = false;
	bool glossyReflection = false;

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;

	bool m_bSceneLoaded;
};

#endif // __RAYTRACER_H__
