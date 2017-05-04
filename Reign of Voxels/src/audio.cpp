#include "Audio.hpp"
#include "ResourceManager.hpp"

Audio::Audio()
{

}


//void Audio::onNotify(Event event, Entity *entity)
//{
//	
//}

void Audio::Init(glm::vec3 position, glm::vec3 forward, int globalVolume)
{
	sf::Listener::setGlobalVolume(globalVolume);
	sf::Listener::setUpVector(0, 1, 0);
	sf::Listener::setPosition(position.x, position.y, position.z);
	sf::Listener::setDirection(forward.x, forward.y, forward.z);
}

void Audio::PlaySound(std::string sound, int volume)
{

}

void Audio::PlayMusic(std::string sound, int volume)
{

}

void Audio::Update(glm::vec3 position, glm::vec3 forward)
{
	sf::Listener::setUpVector(0, 1, 0);
	sf::Listener::setPosition(position.x, position.y, position.z);
	sf::Listener::setDirection(forward.x, forward.y, forward.z);

}

void Audio::SetVolume(float volume)
{

}

