#pragma once
#include "graphics.h"
#include "EventSystem.h"

#define MAX_SCENES 10
#define MAX_PENDING_EVENTS 100

class Scene : public Observer
{
public:
	virtual ~Scene() {};
	virtual void SceneFrame() = 0;
	virtual void Render() = 0;
	virtual void HandleInput(sf::Event event) = 0;
};

class SceneManager : public Observer
{
public:
	SceneManager(Scene *scene);
	~SceneManager();
	void pushScene(Scene * scene);
	void popScene();
	void SceneFrame();
	void HandleEvents();

	virtual void onNotify(Event event, Json &data);
private:
	//ring buffer event queue
	Json			m_pending_events[100];
	int				m_event_head,
					m_event_tail;
	//input queue for only top scene
	Scene			*m_scenes[MAX_SCENES];//scene stack
	int				m_numScenes; // number of scenes on stack
	//commands can be notified immediately

};

