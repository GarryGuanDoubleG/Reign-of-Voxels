#pragma once
#include "SFML\Graphics.hpp"
#include "scene.h"
#include "model.h"
#include "camera.h"
#include "game.h"
#include "Voxel.h"

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

	virtual void SceneFrame();
	virtual void Render() ;
	virtual void onNotify(Event event, sf::Event &input);
	virtual void HandleInput(sf::Event event) ;
private:
	static Voxel *m_Voxels[32][32][32];

	Model *m_model;
	Camera *m_camera;
};