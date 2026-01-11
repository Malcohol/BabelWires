#include <gtest/gtest.h>

#include <BabelWiresLib/Instance/instance.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Processors/parallelProcessor.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testParallelProcessor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    bool findPath(const std::string& log, const babelwires::ValueTreeNode& f) {
        const babelwires::Path path = babelwires::getPathTo(&f);
        std::ostringstream pathStream;
        pathStream << path;
        std::string pathString = pathStream.str();
        return log.find(pathString) != log.npos;
    }
} // namespace

TEST(ParallelProcessorTest, updateOutputOnChanges) {
    // Use the testEnvironment's log to determine when the processEntry method is called.
    testUtils::TestEnvironment testEnvironment;


    testDomain::TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInput().setToDefault();
    processor.getOutput().setToDefault();

    babelwires::ValueTreeNode& input = processor.getInput();
    const babelwires::ValueTreeNode& output = processor.getOutput();

    babelwires::ValueTreeNode& intValueTreeNode =
        processor.getInput().getChildFromStep(babelwires::PathStep("intVal")).as<babelwires::ValueTreeNode>();

    babelwires::ValueTreeNode& inputArrayTreeNode =
        input.getChildFromStep(testDomain::TestParallelProcessor::getCommonArrayId()).as<babelwires::ValueTreeNode>();
    const babelwires::ValueTreeNode& outputArrayTreeNode =
        output.getChildFromStep(testDomain::TestParallelProcessor::getCommonArrayId()).as<babelwires::ValueTreeNode>();

    babelwires::ArrayInstanceImpl<babelwires::ValueTreeNode, babelwires::IntType> inputArray(inputArrayTreeNode);
    const babelwires::ArrayInstanceImpl<const babelwires::ValueTreeNode, babelwires::IntType> outputArray(
        outputArrayTreeNode);

    EXPECT_EQ(inputArray.getSize(), 1);
    EXPECT_EQ(outputArray.getSize(), 1);

    EXPECT_EQ(inputArray.getEntry(0).get(), 0);
    EXPECT_EQ(outputArray.getEntry(0).get(), 0);

    processor.getInput().clearChanges();
    intValueTreeNode.setValue(babelwires::IntValue(1));
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(outputArray.getEntry(0).get(), 1);

    processor.getInput().clearChanges();
    inputArray.getEntry(0).set(2);
    processor.process(testEnvironment.m_log);
    EXPECT_EQ(outputArray.getEntry(0).get(), 3);

    processor.getInput().clearChanges();
    inputArray.setSize(2);
    processor.process(testEnvironment.m_log);

    EXPECT_EQ(outputArray.getSize(), 2);
    EXPECT_EQ(outputArray.getEntry(0).get(), 3);
    EXPECT_EQ(outputArray.getEntry(1).get(), 1);

    processor.getInput().clearChanges();
    inputArray.setSize(1);
    processor.process(testEnvironment.m_log);

    EXPECT_EQ(outputArray.getSize(), 1);
    EXPECT_EQ(outputArray.getEntry(0).get(), 3);
}

TEST(ParallelProcessorTest, noUnnecessaryWorkDone) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInput().setToDefault();
    processor.getOutput().setToDefault();

    babelwires::ValueTreeNode& input = processor.getInput();
    const babelwires::ValueTreeNode& output = processor.getOutput();

    babelwires::ValueTreeNode& intValueTreeNode =
        processor.getInput().getChildFromStep(babelwires::PathStep("intVal")).as<babelwires::ValueTreeNode>();

    babelwires::ValueTreeNode& inputArrayTreeNode =
        input.getChildFromStep(testDomain::TestParallelProcessor::getCommonArrayId()).as<babelwires::ValueTreeNode>();
    const babelwires::ValueTreeNode& outputArrayTreeNode =
        output.getChildFromStep(testDomain::TestParallelProcessor::getCommonArrayId()).as<babelwires::ValueTreeNode>();

    babelwires::ArrayInstanceImpl<babelwires::ValueTreeNode, babelwires::IntType> inputArray(inputArrayTreeNode);
    const babelwires::ArrayInstanceImpl<const babelwires::ValueTreeNode, babelwires::IntType> outputArray(
        outputArrayTreeNode);

    processor.getInput().clearChanges();
    {
        intValueTreeNode.setValue(babelwires::IntValue(4));
        inputArray.setSize(2);
        inputArray.getEntry(0).set(5);
        inputArray.getEntry(1).set(6);
    }
    processor.process(testEnvironment.m_log);

    processor.getInput().clearChanges();
    testEnvironment.m_log.clear();
    EXPECT_EQ(testEnvironment.m_log.getLogContents(), "");

    processor.process(testEnvironment.m_log);
    EXPECT_EQ(testEnvironment.m_log.getLogContents(), "");

    processor.getInput().clearChanges();
    testEnvironment.m_log.clear();

    processor.getInput().clearChanges();
    inputArray.getEntry(0).set(7);
    processor.process(testEnvironment.m_log);

    EXPECT_EQ(outputArray.getSize(), 2);
    EXPECT_EQ(outputArray.getEntry(0).get(), 11);
    EXPECT_EQ(outputArray.getEntry(1).get(), 10);

    EXPECT_TRUE(findPath(testEnvironment.m_log.getLogContents(), *inputArray.getEntry(0)));
    EXPECT_FALSE(findPath(testEnvironment.m_log.getLogContents(), *inputArray.getEntry(1)));

    processor.getInput().clearChanges();
    testEnvironment.m_log.clear();
    inputArray.getEntry(0).set(0);
    processor.process(testEnvironment.m_log);

    EXPECT_EQ(outputArray.getSize(), 2);
    EXPECT_EQ(outputArray.getEntry(0).get(), 4);
    EXPECT_EQ(outputArray.getEntry(1).get(), 10);

    EXPECT_TRUE(findPath(testEnvironment.m_log.getLogContents(), *inputArray.getEntry(0)));
    EXPECT_FALSE(findPath(testEnvironment.m_log.getLogContents(), *inputArray.getEntry(1)));

    // TODO: Adjusting the size of the array (or inserting / deleting elements) should also not trigger work,
    // but this needs work.
}

TEST(ParallelProcessorTest, testFailure) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInput().setToDefault();
    processor.getOutput().setToDefault();

    babelwires::ValueTreeNode& input = processor.getInput();
    const babelwires::ValueTreeNode& output = processor.getOutput();

    babelwires::ValueTreeNode& intValueTreeNode =
        processor.getInput().getChildFromStep(babelwires::PathStep("intVal")).as<babelwires::ValueTreeNode>();

    babelwires::ValueTreeNode& inputArrayTreeNode =
        input.getChildFromStep(testDomain::TestParallelProcessor::getCommonArrayId()).as<babelwires::ValueTreeNode>();
    const babelwires::ValueTreeNode& outputArrayTreeNode =
        output.getChildFromStep(testDomain::TestParallelProcessor::getCommonArrayId()).as<babelwires::ValueTreeNode>();

    babelwires::ArrayInstanceImpl<babelwires::ValueTreeNode, babelwires::IntType> inputArray(inputArrayTreeNode);
    const babelwires::ArrayInstanceImpl<const babelwires::ValueTreeNode, babelwires::IntType> outputArray(
        outputArrayTreeNode);

    EXPECT_EQ(inputArray.getSize(), 1);
    EXPECT_EQ(outputArray.getSize(), 1);

    EXPECT_EQ(inputArray.getEntry(0).get(), 0);
    EXPECT_EQ(outputArray.getEntry(0).get(), 0);

    processor.getInput().clearChanges();
    intValueTreeNode.setValue(babelwires::IntValue(4));
    inputArray.setSize(2);
    inputArray.getEntry(0).set(17);
    inputArray.getEntry(1).set(6);

    try {
        processor.process(testEnvironment.m_log);
        EXPECT_FALSE(true);
    } catch (const babelwires::ModelException& e) {
        EXPECT_TRUE(findPath(e.what(), *inputArray.getEntry(0)));
        EXPECT_FALSE(findPath(e.what(), *inputArray.getEntry(1)));
    } catch (...) {
        ASSERT_FALSE(false);
    }
}
