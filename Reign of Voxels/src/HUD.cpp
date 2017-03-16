#include "game.hpp"
#include "HUD.hpp"

#include "VoxelManager.hpp"

HUD::HUD()
{

}


void HUD::DrawMiniMap()
{
	glEnable(GL_CULL_FACE);

	//manager->RenderMinimap(m_minimapCam);

	glDisable(GL_CULL_FACE);
}

//load textures and fonts
void HUD::Init()
{

}

void HUD::HandleInput(sf::Event event)
{

}