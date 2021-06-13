#pragma once

#include <vector>
#include <optional>
#include <memory>
#include <atomic>
#include <string>
#include <filesystem>

#include <misc/Misc.h>

struct Mix_Chunk;

namespace sound
{
	enum class Sample
	{
		BUTTON_CLICK,
		TEXT_EDIT,
		TEXT_DELETE,
		BUTTON_HOVER,
		MAX
	};

	struct SoundStore
	{
		Mix_Chunk* data;
		float duration;

		std::vector<double> plays;

		std::optional<int> playChannel(int channel, int volume, int max, int loops);
	};

	struct SoundPlayer
	{
		bool valid = false;
		static std::optional<std::unique_ptr<SoundPlayer>> makeSoundPlayer(int frequency, int channels, bool stereo = true);

		int channels = 0;
		int frequency = 0;

		const int bytesPerSample = 4;

		std::array<SoundStore, static_cast<size_t>(Sample::MAX)> sounds;

		bool loadSound(Sample sample, std::filesystem::path const& sound);
		bool playSound(Sample sample, int volume = 128, int max = 0, int loops = 0);

		SoundPlayer() = default;
		~SoundPlayer();

		NO_COPY_MOVE(SoundPlayer);
	};
}