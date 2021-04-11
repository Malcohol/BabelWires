/**
 * AlsaAudioInterface is an AudioInterface for ALSA (Advanced Linux Sound System).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "Audio/Alsa/alsaAudioInterface.hpp"
#include "Audio/Alsa/alsaAudioDest.hpp"
#include "Audio/Alsa/alsaAudioSource.hpp"
#include "Audio/Alsa/alsaCommon.hpp"
#include <alsa/asoundlib.h>

namespace {
    const char s_identifier[] = "Alsa";
    const char s_name[] = "Alsa";
} // namespace

babelwires_alsa::AlsaAudioInterface::AlsaAudioInterface()
    : babelwires::AudioInterface(s_identifier, s_name, 1) {}

const std::string babelwires_alsa::AlsaAudioInterface::getThisIdentifier() {
    return s_identifier;
}

namespace {
    struct AlsaNameQuery {
        AlsaNameQuery() {
            const int ret = snd_device_name_hint(-1, "pcm", &m_hints);
            babelwires_alsa::checkForError("Trying to query device names", ret);
        }

        enum class Direction { Destination, Source };

        std::vector<std::string> getNames(Direction d) const {
            std::vector<std::string> names;
            void** hintptr = m_hints;
            while (*hintptr != nullptr) {
                const char* name = snd_device_name_get_hint(*hintptr, "NAME");
                const char* ioid = snd_device_name_get_hint(*hintptr, "IOID");
                if (!ioid // Input and output
                    || ((strcmp(ioid, "Output") == 0) && (d == Direction::Destination)) ||
                    ((strcmp(ioid, "Input") == 0) && (d == Direction::Source))) {
                    names.emplace_back(name);
                }
                ++hintptr;
            }
            return names;
        }

        ~AlsaNameQuery() { snd_device_name_free_hint(m_hints); }

        void** m_hints = nullptr;
    };
} // namespace

std::vector<std::string> babelwires_alsa::AlsaAudioInterface::getDestinationNames() const {
    AlsaNameQuery nameQueryObject;
    return nameQueryObject.getNames(AlsaNameQuery::Direction::Destination);
}

std::unique_ptr<babelwires::AudioDest>
babelwires_alsa::AlsaAudioInterface::getDestination(std::string_view destinationName) const {
    std::string destName(destinationName);
    return std::make_unique<AlsaAudioDest>(destName.c_str());
}

std::vector<std::string> babelwires_alsa::AlsaAudioInterface::getSourceNames() const {
    AlsaNameQuery nameQueryObject;
    return nameQueryObject.getNames(AlsaNameQuery::Direction::Source);
}

std::unique_ptr<babelwires::AudioSource>
babelwires_alsa::AlsaAudioInterface::getSource(std::string_view sourceName) const {
    std::string srcName(sourceName);
    return std::make_unique<AlsaAudioSource>(srcName.c_str());
}
