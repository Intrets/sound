#include "SoundPlayer.h"

#include <iostream>
#include <chrono>

#include <SDL_mixer.h>
#include <SDL.h>

#include <mem/Global.h>

#include <misc/PathManager.h>

namespace sound
{
	std::optional<int> SoundStore::playChannel(int channel, int volume, int max, int loops) {
		const TimePoint current = std::chrono::system_clock::now();
		const TimePoint newEnd = current + (this->duration * (loops + 1));

		std::erase_if(this->plays, [=](auto e) {return current > e; });
		if (max > 0 && this->plays.size() >= max) {
			return std::nullopt;
		}
		else {
			auto c = Mix_PlayChannel(channel, this->data, loops);

			if (c == -1) {
				return std::nullopt;
			}
			else {
				Mix_Volume(c, volume);
				this->plays.push_back(newEnd);
				return c;
			}
		}
	}

	std::optional<std::unique_ptr<SoundPlayer>> SoundPlayer::makeSoundPlayer(int frequency, int channels, bool stereo) {
		if (SDL_Init(SDL_INIT_AUDIO) == -1) {
			return std::nullopt;
		}

		auto res = std::make_unique<SoundPlayer>();

		if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
			SDL_Quit();
			return std::nullopt;
		}

		if (Mix_Init(MIX_INIT_OGG) == 0) {
			Mix_Quit();
			SDL_Quit();
			return std::nullopt;
		}

		res->valid = true;
		res->channels = Mix_AllocateChannels(channels);
		res->frequency = frequency;

		auto soundsPath = Global<misc::PathManager>->getSoundsPath();

		res->loadSound(Sample::BUTTON_CLICK, soundsPath / "button.oga");
		res->loadSound(Sample::BUTTON_HOVER, soundsPath / "hover.oga");
		res->loadSound(Sample::TEXT_EDIT, soundsPath / "text_edit.oga");
		res->loadSound(Sample::TEXT_DELETE, soundsPath / "text_delete.oga");

		return res;
	}

	bool SoundPlayer::loadSound(Sample sample, std::filesystem::path const& sound) {
		assert(this->valid);
		if (!this->valid) {
			return false;
		}
		auto path = sound.string();
		auto chunk = Mix_LoadWAV(path.c_str());
		if (chunk == nullptr) {
			return false;
		}
		else {
			this->sounds[static_cast<size_t>(sample)].data = chunk;
			auto duration = static_cast<float>(chunk->alen) / static_cast<float>(this->frequency) / static_cast<float>(this->bytesPerSample);
			this->sounds[static_cast<size_t>(sample)].duration = std::chrono::milliseconds(static_cast<int>(duration * 1000.0f));
			return true;
		}
	}

	bool SoundPlayer::playSound(Sample sample, int volume, int max, int loops) {
		assert(this->valid);
		if (!this->valid) {
			return false;
		}

		assert(sample != Sample::MAX);
		auto index = static_cast<size_t>(sample);

		return this->sounds[index].playChannel(-1, volume, max, loops).has_value();
	}

	SoundPlayer::~SoundPlayer() {
		if (this->valid) {
			Mix_CloseAudio();

			for (auto& sound : this->sounds) {
				Mix_FreeChunk(sound.data);
			}

			Mix_Quit();
			SDL_Quit();
		}
	}
}