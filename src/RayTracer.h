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
	bool sceneLoaded();
	Scene* getScene();

	//background
	void loadBackground(char* img);
	vec3f getBackground(double x, double y);
	bool useBackground = false;


	//texture 
	void loadTexture(char* img);
	bool useTexture= false;
	
	//bummping 
	void loadBumpping(char* Img);
	bool useBump = false;

	// Recursion depth
	int maxDepth = 0;

	// Anti-aliasing
	bool useSuperSampling = false;
	bool useJitter = false;
	int numSuperSample = 1;
	bool useAdaptiveSampling = false;
	vec3f adaptiveSampling(const double x, const double y, int depth);

private:
	//buffer
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	//background
	unsigned char* background;
	int background_height, background_width;
	//texture
	unsigned char* textureImg;
	int texture_height, texture_width;
	//bumpping 
	unsigned char* bumpping;
	int bump_height, bump_width;
	
	Scene *scene;
	

	bool m_bSceneLoaded;
};

#endif // __RAYTRACER_H__
