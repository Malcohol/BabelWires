#include <gtest/gtest.h>

#include "Audio/Alsa/alsaAudioInterface.hpp"
#include "Audio/Alsa/alsaInit.hpp"
#include "Common/Audio/audioDest.hpp"
#include "Common/Audio/audioInterface.hpp"
#include "Common/Audio/audioSource.hpp"

#include <array>
#include <numeric>

struct AlsaTest : public ::testing::Test {
    void SetUp() override { babelwires_alsa::init_audio(m_audioReg); }

    void TearDown() override { babelwires_alsa::shutdown_audio(m_audioReg); }

    babelwires::AudioInterfaceRegistry m_audioReg;
};

TEST_F(AlsaTest, regTest) {
    // Test will need to be adjusted if this changes.
    ASSERT_EQ(babelwires_alsa::AlsaAudioInterface::getThisIdentifier(), "Alsa");

    // Check that the alsa plugin registered itself.
    const babelwires::AudioInterface* alsaInterface =
        m_audioReg.getEntryByIdentifier(babelwires_alsa::AlsaAudioInterface::getThisIdentifier());
    EXPECT_TRUE(alsaInterface);

    // Check that the null destination, at least, is present.
    {
        auto destinationNames = m_audioReg.getDestinationNames();
        EXPECT_GT(destinationNames.size(), 0);
        EXPECT_TRUE(std::find(destinationNames.begin(), destinationNames.end(), "Alsa::null") !=
                    destinationNames.end());
    }

    // Check that the null source, at least, is present.
    {
        auto sourceNames = m_audioReg.getSourceNames();
        EXPECT_GT(sourceNames.size(), 0);
        EXPECT_TRUE(std::find(sourceNames.begin(), sourceNames.end(), "Alsa::null") != sourceNames.end());
    }
}

// In lieu of creating a test alsa device, do some minimal testing using null devices.
TEST_F(AlsaTest, nullSource) {
    auto audioSource = m_audioReg.getSource("Alsa::null");
    ASSERT_TRUE(audioSource);

    EXPECT_TRUE(audioSource->getNumChannels() > 0);

    std::array<babelwires::AudioSample, 1024> dstBuffer;
    std::iota(dstBuffer.begin(), dstBuffer.end(), 0);

    const unsigned long numSamplesObtained = audioSource->getMoreAudioData(dstBuffer.data(), dstBuffer.size());
    EXPECT_EQ(numSamplesObtained, dstBuffer.size());
}

// In lieu of creating a test alsa device, do some minimal testing using null devices.
TEST_F(AlsaTest, nullDest) {
    auto audioDest = m_audioReg.getDestination("Alsa::null");
    ASSERT_TRUE(audioDest);

    EXPECT_TRUE(audioDest->getNumChannels() > 0);

    std::array<babelwires::AudioSample, 1024> srcBuffer;
    std::iota(srcBuffer.begin(), srcBuffer.end(), 0);

    const unsigned long numSamplesUsed = audioDest->writeMoreAudioData(srcBuffer.data(), srcBuffer.size());
    EXPECT_EQ(numSamplesUsed, srcBuffer.size());
}
