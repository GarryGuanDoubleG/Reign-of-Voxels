#include "game.hpp"
#include "HUD.hpp"

#include "VoxelManager.hpp"

HUD::HUD()
{
//	m_minimapCam = new Camera();
	/*m_minimapCam->SetToOrtho();
	m_minimapCam->SetPosition(Vec3(256 * 16, 512, 256 * 16));*/
}


void HUD::DrawMiniMap()
{
	glEnable(GL_CULL_FACE);

	//manager->RenderMinimap(m_minimapCam);

	glDisable(GL_CULL_FACE);
}

void HUD::HandleInput(sf::Event event)
{

}