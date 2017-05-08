#include "Audio.hpp"
#include "ResourceManager.hpp"

#define SOUND_PATH "Resources\\sounds\\"

Audio::Audio()
{
	m_channel = 0;
}

void Audio::onNotify(Event event, std::string sound)
{
	switch (event)
	{
	case Sound:
		PlaySound(sound, 100);
		break;
	case Music:
		PlayMusic(sound, 33);
		break;
	}
}

void Audio::Init(glm::vec3 position, glm::vec3 forward, int globalVolume)
{
	sf::Listener::setGlobalVolume(globalVolume);
	sf::Listener::setUpVector(0, 1, 0);
	sf::Listener::setPosition(position.x, position.y, position.z);
	sf::Listener::setDirection(forward.x, forward.y, forward.z);
	sf::Listener::setGlobalVolume(100);
}

void Audio::PlaySound(std::string sound, int volume)
{
	std::string path = GetSoundDef(sound);
	m_buffer[m_channel].loadFromFile(path);

	m_sounds[m_channel] = sf::Sound(m_buffer[m_channel]);

	m_sounds[m_channel].play();

	m_channel++;

	//circular array loop back to beginning
	if (m_channel > (sizeof(m_sounds) / sizeof(sf::Sound)));
	{
		m_channel = 0;
	}
}

void Audio::PlayMusic(std::string sound, int volume)
{
	std::string path = GetMusicDef(sound);

	if (!m_music.openFromFile(path))
	{
		return;
	}
	m_music.setVolume(100);
	m_music.play();
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

