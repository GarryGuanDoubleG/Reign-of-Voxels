#pragma once
#include "graphics.h"
#include "EventSystem.h"

#define MAX_SCENES 10
class Scene : public Observer
{
public:
	virtual void SceneLoop() = 0;
	virtual void Render() = 0;
	virtual void HandleInput(sf::Event event) = 0;
};

class SceneManager
{
public:
	void pushScene(Scene * scene);
	void popScene();
	void RunScene();
private:
	//input queue for only top scene
	Scene		*m_scenes[MAX_SCENES];//scene stack
	Scene		*m_head,
				*m_tail;
	int			m_numScenes; // number of scenes on stack
	//commands can be notified immediately

};

