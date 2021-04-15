#ifndef __BOOLEAN_H__
#define __BOOLEAN_H__

#include "../scene/scene.h"
#include "../scene/ray.h"

class BooleanNode : public SceneObject
{
public:
	BooleanNode(Scene* scene, SceneObject* a, SceneObject* b)
		: SceneObject(scene), a(a), b(b) {}
	~BooleanNode() { delete a; delete b; }

	const Material& getMaterial() const override;
	void setMaterial(Material* m) override;

	SceneObject* a;
	SceneObject* b;
};

class UnionNode : public BooleanNode
{
public:
	UnionNode(Scene* scene, SceneObject* a, SceneObject* b)
		: BooleanNode(scene, a, b) {}
	bool intersect(const ray& r, isect& i) const;

};

class IntersectionNode : public BooleanNode
{
public:
	IntersectionNode(Scene* scene, SceneObject* a, SceneObject* b)
		: BooleanNode(scene, a, b) {}
	bool intersect(const ray& r, isect& i) const;

};

class SubtractNode : public BooleanNode
{
public:
	SubtractNode(Scene* scene, SceneObject* a, SceneObject* b)
		: BooleanNode(scene, a, b) {}
	bool intersect(const ray& r, isect& i) const;

};

#endif // __BOOLEAN_H__