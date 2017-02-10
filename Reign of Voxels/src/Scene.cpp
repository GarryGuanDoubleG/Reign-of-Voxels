#include "scene.h"
#include "Gui\menu.h"

SceneManager::SceneManager(Scene *scene)
{
	m_numScenes = 0;
	pushScene(scene);
}

void SceneManager::pushScene(Scene * scene)
{
	m_scenes[m_numScenes++] = scene;
}

void SceneManager::popScene()
{
	m_scenes[--m_numScenes] = NULL;
}

void SceneManager::RunScene()
{
	while (g_window->isOpen())
	{
		m_scenes[m_numScenes - 1]->SceneLoop();
	}
}

void SceneManager::onNotify(Event event)
{
	m_pending_events[m_event_head++] = event;
	if (m_event_head >= MAX_PENDING_EVENTS)
	{
		m_event_head = 0;//reset head to beginning of array
	}



	switch (event)
	{
	case JoinLobby:
	{
		pushScene(new Menu(LobbyMenu));
		break;
	}
	default:
		break;
	}
}