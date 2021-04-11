#include "Common/Audio/fileAudioDest.hpp"
#include "Common/exceptions.hpp"
#include <assert.h>
#include <stdexcept>

#include <sndfile.h>

namespace {

    struct SndFileAudioDest : babelwires::AudioDest {
        SndFileAudioDest(const char* fileName, std::uint32_t formatCode, unsigned int numChannels) {
            assert(numChannels > 0);
            assert(numChannels <= 2);

            m_info.channels = numChannels;
            m_info.samplerate = 44100;
            m_info.format = formatCode;
            m_sndFile = sf_open(fileName, SFM_WRITE, &m_info);

            if (m_sndFile) {

            } else {
                throw babelwires::FileIoException() << getErrorString();
            }
        }

        ~SndFileAudioDest() {
            if (m_sndFile) {
                sf_close(m_sndFile);
            }
        }

        int getNumChannels() const { return m_info.channels; }

        babelwires::Duration getFrequency() const { return m_info.samplerate; }

        unsigned long writeMoreAudioData(const babelwires::AudioSample* buffer, unsigned long bufSize) {
            assert(sf_error(m_sndFile) == SF_ERR_NO_ERROR);
            return sf_write_float(m_sndFile, buffer, bufSize);
        }

        std::string getErrorString() {
            assert(sf_error(m_sndFile) != SF_ERR_NO_ERROR);
            return sf_strerror(m_sndFile);
        }

        SF_INFO m_info;
        SNDFILE* m_sndFile;
    };

    struct SndFileAudioDestFactory : babelwires::FileAudioDestFactory {
        SndFileAudioDestFactory(const char* id, const char* name, const char* ext, std::uint32_t code)
            : FileAudioDestFactory(id, name, 1, Extensions{ext})
            , m_formatCode(code) {}

        virtual std::unique_ptr<babelwires::AudioDest> createFileAudioDest(const char* fileName,
                                                                           unsigned int numChannels) const override {
            return std::make_unique<SndFileAudioDest>(fileName, m_formatCode, numChannels);
        }

        std::uint32_t m_formatCode;
    };

} // namespace

babelwires::FileAudioDestFactory::FileAudioDestFactory(std::string identifier, std::string name, VersionNumber version,
                                                       Extensions extensions)
    : FileTypeEntry(std::move(identifier), std::move(name), version, std::move(extensions)) {}

babelwires::FileAudioDestRegistry::FileAudioDestRegistry()
    : FileTypeRegistry<FileAudioDestFactory>("File Audio Dest Registry") {
    addEntry(std::make_unique<SndFileAudioDestFactory>("WAV", "WAV file", "wav", (SF_FORMAT_WAV | SF_FORMAT_PCM_16)));
    addEntry(
        std::make_unique<SndFileAudioDestFactory>("AIFF", "Aiff file", "aiff", (SF_FORMAT_AIFF | SF_FORMAT_PCM_16)));
    addEntry(std::make_unique<SndFileAudioDestFactory>("FLAC", "FLAC file", "flac", SF_FORMAT_FLAC));
    addEntry(std::make_unique<SndFileAudioDestFactory>("OGG", "OGG file", "ogg", SF_FORMAT_OGG));
}

std::unique_ptr<babelwires::AudioDest>
babelwires::FileAudioDestRegistry::createFileAudioDest(const char* fileName, unsigned int numChannels) const {
    if (const auto* factory = getEntryByFileName(fileName)) {
        return factory->createFileAudioDest(fileName, numChannels);
    } else {
        throw FileIoException() << "The file name \"" << fileName
                                << "\" is not in a recognized audio destination format";
    }
}
