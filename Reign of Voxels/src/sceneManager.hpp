#pragma once
#include "scene.hpp"
/**
* Manages which scene is activately handles events using a stack
*/

class SceneManager : public Observer
{
public:
	/** Constructor
	* @brief initalizes members to 0 and pushes a new scene onto the stack
	* @param scene the scene to push onto scene stack
	*/
	SceneManager(Scene *scene);
	/** Destructor
	* @brief pops off all scenes on stack and deletes them.
	* unsubscribes from event system
	*/
	~SceneManager();
	/**
	* @brief pushes scene onto scene stack
	* @param scene to push
	*/
	void pushScene(Scene * scene);
	/**
	* @brief delets scene at top of stack and pops it off
	*/
	void popScene();
	/**
	* @brief Runs sceneframe method of scene on top of stack
	*/
	void SceneFrame();
	/**
	* @brief Pops or pushes states based on events queued up
	*/
	void HandleEvents();
	/**
	* @brief Queues up events to handle next frame
	* @param event the event that happened
	* @param data about the event
	*/
	virtual void onNotify(Event event, Json &data);
private:
	Json			m_pending_events[100]; /**<ring buffer queue of json data from events */
	int				m_event_head, /**< index of queue head */
		m_event_tail; /**<  index of queue tail*/
					  //input queue for only top scene
	Scene			*m_scenes[MAX_SCENES]; /**<Scene stack*/
	int				m_numScenes; /**< number of scenes in stack.*/
};

