#include <gtest/gtest.h>

#include <BabelWiresLib/Processors/parallelProcessor.hpp>

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>
#include <BabelWiresLib/Features/featureMixins.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    using LimitedIntFeature = babelwires::HasStaticRange<babelwires::IntFeature, -20, 20>;

    struct TestParallelProcessor : babelwires::ParallelProcessor<LimitedIntFeature, LimitedIntFeature> {
        TestParallelProcessor(const babelwires::ProjectContext& context) : babelwires::ParallelProcessor<LimitedIntFeature, LimitedIntFeature>(context) {
            const babelwires::ShortId intId = babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
                "foo", "foo", "ec463f45-098d-4170-9890-d5a2db2e7658",
                babelwires::IdentifierRegistry::Authority::isAuthoritative);
            const babelwires::ShortId arrayId = babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
                "array", "array", "d607ccca-7cc3-4d17-940e-3ca87467f064",
                babelwires::IdentifierRegistry::Authority::isAuthoritative);
            m_intValue = m_inputFeature->addField(std::make_unique<babelwires::IntFeature>(), intId);
            addArrayFeature(arrayId);
        }

        virtual void processEntry(babelwires::UserLogger& userLogger, const InputEntryFeature& input,
                                  OutputEntryFeature& output) const override {
            userLogger.logInfo() << babelwires::FeaturePath(&input);
            output.set(input.get() + m_intValue->get());
        }

        babelwires::IntFeature* m_intValue;
    };

    bool findPath(const std::string& log, const babelwires::Feature* f) {
        const babelwires::FeaturePath path(f);
        std::ostringstream pathStream;
        pathStream << path;
        std::string pathString = pathStream.str();
        return log.find(pathString) != log.npos;
    }
} // namespace

TEST(ParallelProcessorTest, updateOutputOnChanges) {
    testUtils::TestEnvironment testEnvironment;

    TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInputFeature()->setToDefault();
    processor.getOutputFeature()->setToDefault();

    babelwires::ArrayFeature* const inputArrayFeature =
        processor.getInputFeature()->getChildFromStep(babelwires::PathStep("array")).as<babelwires::ArrayFeature>();
    const babelwires::ArrayFeature* const outputArrayFeature =
        processor.getOutputFeature()->getChildFromStep(babelwires::PathStep("array")).as<babelwires::ArrayFeature>();

    const auto getInputArrayEntry = [inputArrayFeature](int i) {
        return inputArrayFeature->getFeature(i)->as<babelwires::IntFeature>();
    };
    const auto getOutputArrayEntry = [outputArrayFeature](int i) {
        return outputArrayFeature->getFeature(i)->as<babelwires::IntFeature>();
    };

    EXPECT_EQ(inputArrayFeature->getNumFeatures(), 1);
    EXPECT_EQ(outputArrayFeature->getNumFeatures(), 1);
    EXPECT_EQ(processor.m_intValue->get(), 0);
    EXPECT_EQ(getInputArrayEntry(0)->get(), 0);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 0);

    processor.m_intValue->set(1);
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 1);

    processor.getInputFeature()->clearChanges();
    getInputArrayEntry(0)->set(2);
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 3);

    processor.getInputFeature()->clearChanges();
    inputArrayFeature->addEntry();
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(outputArrayFeature->getNumFeatures(), 2);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 3);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 1);

    processor.getInputFeature()->clearChanges();
    inputArrayFeature->removeEntry(0);
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(outputArrayFeature->getNumFeatures(), 1);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 1);
}

TEST(ParallelProcessorTest, noUnnecessaryWorkDone) {
    // Use the testEnvironment's log to determine when the processEntry method is called.
    testUtils::TestEnvironment testEnvironment;

    TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInputFeature()->setToDefault();
    processor.getOutputFeature()->setToDefault();

    babelwires::ArrayFeature* const inputArrayFeature =
        processor.getInputFeature()->getChildFromStep(babelwires::PathStep("array")).as<babelwires::ArrayFeature>();
    const babelwires::ArrayFeature* const outputArrayFeature =
        processor.getOutputFeature()->getChildFromStep(babelwires::PathStep("array")).as<babelwires::ArrayFeature>();

    const auto getInputArrayEntry = [inputArrayFeature](int i) {
        return inputArrayFeature->getFeature(i)->as<babelwires::IntFeature>();
    };
    const auto getOutputArrayEntry = [outputArrayFeature](int i) {
        return outputArrayFeature->getFeature(i)->as<babelwires::IntFeature>();
    };

    processor.m_intValue->set(4);
    inputArrayFeature->addEntry();
    getInputArrayEntry(0)->set(5);
    getInputArrayEntry(1)->set(6);
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 9);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 10);

    processor.getInputFeature()->clearChanges();
    testEnvironment.m_log.clear();
    EXPECT_EQ(testEnvironment.m_log.getLogContents(), "");
   
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(testEnvironment.m_log.getLogContents(), "");
    
    processor.getInputFeature()->clearChanges();
    testEnvironment.m_log.clear();
    getInputArrayEntry(0)->set(7);
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 11);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 10);
    EXPECT_TRUE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(0)));
    EXPECT_FALSE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(1)));

    processor.getInputFeature()->clearChanges();
    testEnvironment.m_log.clear();
    inputArrayFeature->addEntry();
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 11);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 10);
    EXPECT_EQ(getOutputArrayEntry(2)->get(), 4);
    EXPECT_FALSE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(0)));
    EXPECT_FALSE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(1)));
    EXPECT_TRUE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(2)));
    
    processor.getInputFeature()->clearChanges();
    testEnvironment.m_log.clear();
    inputArrayFeature->removeEntry(1);
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 11);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 4);
    EXPECT_FALSE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(0)));
    EXPECT_FALSE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(1)));

    processor.getInputFeature()->clearChanges();
    testEnvironment.m_log.clear();
    processor.m_intValue->set(2);
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 9);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 2);
    EXPECT_TRUE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(0)));
    EXPECT_TRUE(findPath(testEnvironment.m_log.getLogContents(), getInputArrayEntry(1)));
}

TEST(ParallelProcessorTest, testFailure) {
    // Use the testEnvironment's log to determine when the processEntry method is called.
    testUtils::TestEnvironment testEnvironment;

    TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInputFeature()->setToDefault();
    processor.getOutputFeature()->setToDefault();

    babelwires::ArrayFeature* const inputArrayFeature =
        processor.getInputFeature()->getChildFromStep(babelwires::PathStep("array")).as<babelwires::ArrayFeature>();
    const babelwires::ArrayFeature* const outputArrayFeature =
        processor.getOutputFeature()->getChildFromStep(babelwires::PathStep("array")).as<babelwires::ArrayFeature>();

    const auto getInputArrayEntry = [inputArrayFeature](int i) {
        return inputArrayFeature->getFeature(i)->as<babelwires::IntFeature>();
    };
    const auto getOutputArrayEntry = [outputArrayFeature](int i) {
        return outputArrayFeature->getFeature(i)->as<babelwires::IntFeature>();
    };

    processor.m_intValue->set(4);
    inputArrayFeature->addEntry();
    getInputArrayEntry(0)->set(17);
    getInputArrayEntry(1)->set(6);
    try {
        processor.process(testEnvironment.m_log);
        EXPECT_FALSE(true);
    } catch (const std::exception& e) {
        EXPECT_TRUE(findPath(e.what(), getInputArrayEntry(0)));
        EXPECT_FALSE(findPath(e.what(), getInputArrayEntry(1)));
    }
}