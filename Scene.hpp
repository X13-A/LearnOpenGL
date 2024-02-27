#ifndef SCENE_HPP
#define SCENE_HPP


class Scene
{
private:

public:
	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void lateUpdate() = 0;
	virtual void draw() = 0;
};

#endif