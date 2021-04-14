// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"

#include <random>

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	std::stack<Material> mats;
	Material air;
	air.index = 1.0;
	mats.push(air);
	vec3f color = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0, mats).clamp();

	if (depthOfField)
	{
		vec3f V = r.getDirection();
		vec3f up(0, 0, 1);
		vec3f u = (V.cross(up));
		vec3f v = (u.cross(V));
		u = (V.cross(v)).normalize();
		double size = 0.025 * this->aperture;

		double f = this->focalLength;
		vec3f focalPoint = r.getPosition() + f * V;

		for (int i = 1; i < 25; i++)
		{
			double dx = (double)rand() / RAND_MAX * 2.0 * size - size;
			double dy = (double)rand() / RAND_MAX * 2.0 * size - size;
			vec3f dir = V + dx * u + dy * v;
			vec3f P0 = focalPoint - f / (dir.dot(V)) * dir;
			ray r1(P0, dir.normalize());
			color += traceRay(scene, r1, vec3f(1.0, 1.0, 1.0), max(0, maxDepth), mats).clamp();
		}
		color /= 25.0;
	}

	if (motionBlur)
	{
		vec3f V = r.getDirection();
		vec3f up(0, 0, 1);
		vec3f u = (V.cross(up));
		vec3f v = (u.cross(V));
		u = (V.cross(v)).normalize();
		vec3f dir = V;
		for (int i = 1; i < 16; i++)
		{
			dir += 0.002 * v;
			ray r1(r.getPosition(), dir.normalize());
			color += traceRay(scene, r1, vec3f(1.0, 1.0, 1.0), 0, mats).clamp();
		}
		color /= 16.0;
	}

	return color;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth, std::stack<Material> materials )
{
	if (depth > maxDepth) return vec3f(0.0, 0.0, 0.0);

	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		vec3f V = r.getDirection();
		vec3f N = i.N;
		vec3f P = r.at(i.t);

		const Material& m = i.getMaterial();

		vec3f phong = m.shade(scene, r, i);

		// Reflection
		vec3f R = V - 2 * V.dot(N) * N;
		R = R.normalize();
		ray reflectRay(P, R);
		vec3f reflectColor = traceRay(scene, reflectRay, thresh, depth + 1, materials);

		if (glossyReflection)
		{
			vec3f up(0, 0, 1);
			vec3f u = (R.cross(up));
			vec3f v = (u.cross(R));
			u = (R.cross(v)).normalize();
			double size = 0.02;
			int rayDepth = max(depth + 1, maxDepth);
			for (int j = 1; j < 25; j++)
			{
				double dx = (double)rand() / RAND_MAX * 2.0 * size - size;
				double dy = (double)rand() / RAND_MAX * 2.0 * size - size;
				vec3f dir = R + dx * u + dy * v;
				ray r1(P, dir.normalize());
				reflectColor += traceRay(scene, r1, thresh, rayDepth, materials);
			}
			reflectColor /= 16.0;
		}

		reflectColor = prod(m.kr, reflectColor);

		// Refraction
		vec3f refractColor(0.0, 0.0, 0.0);
		if (m.kt.length_squared() > 0) // not opaque
		{
			double n1, n2;
			if (materials.top().uid != m.uid) {
				n1 = materials.top().index;
				n2 = m.index;
				materials.push(m);
			}
			else
			{
				materials.pop();
				n1 = m.index;
				n2 = materials.top().index;
			}

			double eta = n1 / n2;
			double d = V * N;
			double cosSq = 1 - eta * eta * (1 - d * d);

			if (cosSq < RAY_EPSILON) 
			{
				// total internal reflection
				refractColor = vec3f(0, 0, 0);
			}
			else
			{
				vec3f n = N;
				if (d < 0)
					d = -d;
				else
					n = -N;

				vec3f T = eta * V + (eta * d - sqrt(cosSq)) * n;
				ray refractRay(P, T);
				refractColor = traceRay(scene, refractRay, thresh, depth + 1, materials);
			}

			refractColor = prod(m.kt, refractColor);
		}

		return phong + reflectColor + refractColor;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );

	scene->softShadow = softShadow;
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	// Anti-aliasing
	if (useSuperSampling)
	{
		vec3f sum;
		double dx = 1.0 / double(buffer_width) / numSuperSample;
		double dy = 1.0 / double(buffer_height) / numSuperSample;

		for (int ii = 0; ii < numSuperSample; ii++) {
			for (int jj = 0; jj < numSuperSample; jj++) {
				double jx = 0.0, jy = 0.0;
				if (useJitter) 
				{
					jx = ((double)rand() / RAND_MAX) - 0.5;
					jy = ((double)rand() / RAND_MAX) - 0.5;
				}
				sum += trace(scene, x + (ii + jx) * dx, y + (jj + jy) * dy);
			}
		}

		col = sum / (numSuperSample * numSuperSample);
	}
	else if (useAdaptiveSampling)
	{

	}
	else
	{
		col = trace(scene, x, y);
	}

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}