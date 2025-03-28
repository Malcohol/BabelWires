#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Processors/parallelProcessor.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Instance/instance.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

namespace {

    babelwires::TypeRef getLimitedIntType() {
        return babelwires::IntTypeConstructor::makeTypeRef(-20, 20, 0);
    }

    babelwires::ShortId getCommonArrayId() {
        return testUtils::getTestRegisteredIdentifier("array");
    }

    class TestParallelProcessorInput : public babelwires::ParallelProcessorInputBase {
      public:
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("TestProcIn"), 1);

        TestParallelProcessorInput()
            : babelwires::ParallelProcessorInputBase(
                  {{testUtils::getTestRegisteredIdentifier("intVal"), getLimitedIntType()}}, getCommonArrayId(),
                  getLimitedIntType()) {}
    };

    class TestParallelProcessorOutput : public babelwires::ParallelProcessorOutputBase {
      public:
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("TestProcOut"), 1);

        TestParallelProcessorOutput()
            : babelwires::ParallelProcessorOutputBase(getCommonArrayId(), getLimitedIntType()) {}
    };

    struct TestParallelProcessor : babelwires::ParallelProcessor {
        TestParallelProcessor(const babelwires::ProjectContext& context)
            : babelwires::ParallelProcessor(context, TestParallelProcessorInput::getThisType(),
                                            TestParallelProcessorOutput::getThisType()) {}

        void processEntry(babelwires::UserLogger& userLogger, const babelwires::ValueTreeNode& input,
                          const babelwires::ValueTreeNode& inputEntry,
                          babelwires::ValueTreeNode& outputEntry) const override {

            {
                // Log the input path.
                userLogger.logInfo() << babelwires::Path(&inputEntry);
            }

            babelwires::ConstInstance<babelwires::IntType> entryIn{inputEntry};
            babelwires::Instance<babelwires::IntType> entryOut{outputEntry};

            const babelwires::ValueTreeNode& intValueTreeNode =
                input.getChildFromStep(babelwires::PathStep("intVal")).is<babelwires::ValueTreeNode>();

            entryOut.set(entryIn.get() + intValueTreeNode.getValue()->is<babelwires::IntValue>().get());
        }
    };

    bool findPath(const std::string& log, const babelwires::ValueTreeNode& f) {
        const babelwires::Path path(&f);
        std::ostringstream pathStream;
        pathStream << path;
        std::string pathString = pathStream.str();
        return log.find(pathString) != log.npos;
    }
} // namespace

TEST(ParallelProcessorTest, updateOutputOnChanges) {
    // Use the testEnvironment's log to determine when the processEntry method is called.
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry<TestParallelProcessorInput>();
    testEnvironment.m_typeSystem.addEntry<TestParallelProcessorOutput>();

    TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInput().setToDefault();
    processor.getOutput().setToDefault();

    babelwires::ValueTreeNode& input = processor.getInput();
    const babelwires::ValueTreeNode& output = processor.getOutput();

    babelwires::ValueTreeNode& intValueTreeNode =
        processor.getInput().getChildFromStep(babelwires::PathStep("intVal")).is<babelwires::ValueTreeNode>();

    babelwires::ValueTreeNode& inputArrayTreeNode =
        input.getChildFromStep(babelwires::PathStep(getCommonArrayId())).is<babelwires::ValueTreeNode>();
    const babelwires::ValueTreeNode& outputArrayTreeNode =
        output.getChildFromStep(babelwires::PathStep(getCommonArrayId())).is<babelwires::ValueTreeNode>();

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
    testEnvironment.m_typeSystem.addEntry<TestParallelProcessorInput>();
    testEnvironment.m_typeSystem.addEntry<TestParallelProcessorOutput>();

    TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInput().setToDefault();
    processor.getOutput().setToDefault();

    babelwires::ValueTreeNode& input = processor.getInput();
    const babelwires::ValueTreeNode& output = processor.getOutput();

    babelwires::ValueTreeNode& intValueTreeNode =
        processor.getInput().getChildFromStep(babelwires::PathStep("intVal")).is<babelwires::ValueTreeNode>();

    babelwires::ValueTreeNode& inputArrayTreeNode =
        input.getChildFromStep(babelwires::PathStep(getCommonArrayId())).is<babelwires::ValueTreeNode>();
    const babelwires::ValueTreeNode& outputArrayTreeNode =
        output.getChildFromStep(babelwires::PathStep(getCommonArrayId())).is<babelwires::ValueTreeNode>();

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
    testEnvironment.m_typeSystem.addEntry<TestParallelProcessorInput>();
    testEnvironment.m_typeSystem.addEntry<TestParallelProcessorOutput>();

    TestParallelProcessor processor(testEnvironment.m_projectContext);
    processor.getInput().setToDefault();
    processor.getOutput().setToDefault();

    babelwires::ValueTreeNode& input = processor.getInput();
    const babelwires::ValueTreeNode& output = processor.getOutput();

    babelwires::ValueTreeNode& intValueTreeNode =
        processor.getInput().getChildFromStep(babelwires::PathStep("intVal")).is<babelwires::ValueTreeNode>();

    babelwires::ValueTreeNode& inputArrayTreeNode =
        input.getChildFromStep(babelwires::PathStep(getCommonArrayId())).is<babelwires::ValueTreeNode>();
    const babelwires::ValueTreeNode& outputArrayTreeNode =
        output.getChildFromStep(babelwires::PathStep(getCommonArrayId())).is<babelwires::ValueTreeNode>();

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