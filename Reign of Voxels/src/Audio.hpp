#include <vector>
#include <map>
#include <SFML/Audio.hpp>

#include "observer.hpp"
#include "ResourceManager.hpp"

class Audio : public Observer
{
public:
	Audio();
	~Audio();
	//void onNotify(Event event, Entity *entity);

	void PlaySound(std::string sound, int volume);
	void PlayMusic(std::string music, int volume);

	void Update(glm::vec3 position, glm::vec3 forward);
	void SetVolume(float volume);

	void onNotify(Event event, std::string sound);

	void Init(glm::vec3 position, glm::vec3 forward, int globalVolume);

private:
	sf::Music m_music;
	sf::Sound m_sounds[32];
	sf::SoundBuffer m_buffer[32];

	int m_channel;
};