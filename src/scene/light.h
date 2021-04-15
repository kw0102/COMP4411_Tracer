#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color,
		double _a = 0, double _b=0, double _c=0)
		: Light( scene, color ), position( pos ), 
		constantAtt(_a), linearAtt(_b), quadricAtt(_c) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

	double constantAtt;
	double linearAtt;
	double quadricAtt;

protected:
	vec3f position;
};

class AmbientLight : public Light
{
public:
	AmbientLight(Scene* scene, const vec3f& color)
		: Light(scene, color) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

};

class SpotLight : public PointLight
{
public:
	SpotLight(Scene* scene, const vec3f& pos, const vec3f& color,
		double _a, double _b, double _c, const vec3f& orien, const double angle)
		: PointLight(scene, pos, color, _a, _b, _c), orientation(orien), angle(angle) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;

protected:
	vec3f orientation;
	double angle;
};
class WarnModelLight : public PointLight {
public:
	enum struct Type
	{
		kSquare,
		kTriangle,
		kStar
	};

	WarnModelLight(Scene* scene, const vec3f& pos, const vec3f& color,
		double _a,double _b,double _c,const vec3f& orientation,const double size) :
		PointLight(scene, pos, color,_a,_b,_c),orientation(orientation),size(size) {
		u = orientation.cross({ 0,1,0 }).normalize();
		v = orientation.cross(u).normalize();
		u = orientation.cross(v).normalize();
		mat4f translate({ 1,0,0,-pos[0] }, { 0,1,0,-pos[1] }, { 0,0,1,-pos[2] }, { 0,0,0,0 });
		mat4f rotate({ u[0],u[1],u[2],0 }, { v[0],v[1],v[2],0 }, { orientation[0],orientation[1],orientation[2],0 }, { 0,0,0,1 });
		mat4f project({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { 0,0,1,0 });
		matrix = project * rotate * translate;
		
	}
	virtual double distanceAttenuation(const vec3f& P) const override;
	void setType(Type type) { this->type = type; }
protected:
	vec3f orientation,u,v;
	Type type = Type::kSquare;
	mat4f matrix;
	double size;
};

#endif // __LIGHT_H__
