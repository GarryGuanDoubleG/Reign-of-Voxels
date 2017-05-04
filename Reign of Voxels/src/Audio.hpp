#include <vector>
#include <map>
#include <SFML/Audio.hpp>

#include "observer.hpp"

class Audio : public Observer
{
public:
	//void onNotify(Event event, Entity *entity);

	void PlaySound(std::string sound, int volume);
	void PlayMusic(std::string music, int volume);

	void Update(glm::vec3 position, glm::vec3 forward);
	void SetVolume(float volume);

	void Init(glm::vec3 position, glm::vec3 forward, int globalVolume);
private:
	sf::Music m_music;
};