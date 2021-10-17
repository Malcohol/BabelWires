#include <gtest/gtest.h>

#include <BabelWiresLib/Processors/parallelProcessor.hpp>

#include <BabelWiresLib/Identifiers/identifierRegistry.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>
#include "BabelWiresLib/Features/featureMixins.hpp"

#include "Tests/TestUtils/testLog.hpp"

namespace {
    using LimitedIntFeature = babelwires::HasStaticRange<babelwires::IntFeature, -20, 20>;

    struct TestParallelProcessor : babelwires::ParallelProcessor<LimitedIntFeature, LimitedIntFeature> {
        TestParallelProcessor() {
            const babelwires::Identifier intId = babelwires::IdentifierRegistry::write()->addFieldName(
                "foo", "foo", "ec463f45-098d-4170-9890-d5a2db2e7658",
                babelwires::IdentifierRegistry::Authority::isAuthoritative);
            const babelwires::Identifier arrayId = babelwires::IdentifierRegistry::write()->addFieldName(
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
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    TestParallelProcessor processor;
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
    processor.process(log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 1);

    processor.getInputFeature()->clearChanges();
    getInputArrayEntry(0)->set(2);
    processor.process(log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 3);

    processor.getInputFeature()->clearChanges();
    inputArrayFeature->addEntry();
    processor.process(log);
    EXPECT_EQ(outputArrayFeature->getNumFeatures(), 2);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 3);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 1);

    processor.getInputFeature()->clearChanges();
    inputArrayFeature->removeEntry(0);
    processor.process(log);
    EXPECT_EQ(outputArrayFeature->getNumFeatures(), 1);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 1);
}

TEST(ParallelProcessorTest, noUnnecessaryWorkDone) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    // Use the log to determine when the processEntry method is called.
    testUtils::TestLogWithListener log;

    TestParallelProcessor processor;
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
    processor.process(log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 9);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 10);

    processor.getInputFeature()->clearChanges();
    log.clear();
    EXPECT_EQ(log.getLogContents(), "");
   
    processor.process(log);
    EXPECT_EQ(log.getLogContents(), "");
    
    processor.getInputFeature()->clearChanges();
    log.clear();
    getInputArrayEntry(0)->set(7);
    processor.process(log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 11);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 10);
    EXPECT_TRUE(findPath(log.getLogContents(), getInputArrayEntry(0)));
    EXPECT_FALSE(findPath(log.getLogContents(), getInputArrayEntry(1)));

    processor.getInputFeature()->clearChanges();
    log.clear();
    inputArrayFeature->addEntry();
    processor.process(log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 11);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 10);
    EXPECT_EQ(getOutputArrayEntry(2)->get(), 4);
    EXPECT_FALSE(findPath(log.getLogContents(), getInputArrayEntry(0)));
    EXPECT_FALSE(findPath(log.getLogContents(), getInputArrayEntry(1)));
    EXPECT_TRUE(findPath(log.getLogContents(), getInputArrayEntry(2)));
    
    processor.getInputFeature()->clearChanges();
    log.clear();
    inputArrayFeature->removeEntry(1);
    processor.process(log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 11);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 4);
    EXPECT_FALSE(findPath(log.getLogContents(), getInputArrayEntry(0)));
    EXPECT_FALSE(findPath(log.getLogContents(), getInputArrayEntry(1)));

    processor.getInputFeature()->clearChanges();
    log.clear();
    processor.m_intValue->set(2);
    processor.process(log);
    EXPECT_EQ(getOutputArrayEntry(0)->get(), 9);
    EXPECT_EQ(getOutputArrayEntry(1)->get(), 2);
    EXPECT_TRUE(findPath(log.getLogContents(), getInputArrayEntry(0)));
    EXPECT_TRUE(findPath(log.getLogContents(), getInputArrayEntry(1)));
}

TEST(ParallelProcessorTest, testFailure) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    // Use the log to determine when the processEntry method is called.
    testUtils::TestLogWithListener log;

    TestParallelProcessor processor;
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
        processor.process(log);
        EXPECT_FALSE(true);
    } catch (const std::exception& e) {
        EXPECT_TRUE(findPath(e.what(), getInputArrayEntry(0)));
        EXPECT_FALSE(findPath(e.what(), getInputArrayEntry(1)));
    }
}