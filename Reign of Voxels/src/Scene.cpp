#include "scene.h"
#include "Gui\menu.h"
#include "scene.h"
SceneManager::SceneManager(Scene *scene)
{
	m_numScenes = 0;
	pushScene(scene);

	m_event_head = 0;
	m_event_tail = 0;
	//set events to -1 for no events
}

void SceneManager::pushScene(Scene * scene)
{
	m_scenes[m_numScenes++] = scene;
}

void SceneManager::popScene()
{
	delete(m_scenes[--m_numScenes]);
	m_scenes[m_numScenes] = NULL;
}

void SceneManager::SceneFrame()
{
	m_scenes[m_numScenes - 1]->SceneFrame();
	HandleEvents();
}

void SceneManager::HandleEvents()
{
	while (m_event_head != m_event_tail)
	{
			Event game_event = m_pending_events[m_event_head]["event"];
			switch (game_event)
			{
			case JoinLobby:
			{
				pushScene(new Menu(LobbyMenu, m_pending_events[m_event_head]));
				break;
			}
			case JoinPlayer:
				break;
			default:
				break;
			}
		m_pending_events[m_event_head++] = nullptr; //clean up and increment head

		if (m_event_head >= MAX_PENDING_EVENTS)
			m_event_head = 0;	
	}
}

void SceneManager::onNotify(Event event, Json &data)
{
	switch (event)
	{
	case JoinLobby:
		m_pending_events[m_event_tail++] = data; //queue up event
		break;
	default:
		break;
	}
	if (m_event_tail >= MAX_PENDING_EVENTS)
		m_event_tail = 0;
}