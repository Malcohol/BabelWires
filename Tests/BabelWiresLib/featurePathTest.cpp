#include <gtest/gtest.h>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>
#include <Common/Log/unifiedLog.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>
#include <Common/exceptions.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(FeaturePathTest, pathConstructFromSteps) {
    std::vector<babelwires::PathStep> steps = {babelwires::PathStep::deserializeFromString("Hello'2"), 13,
                                               babelwires::PathStep::deserializeFromString("Hello'3"),
                                               babelwires::PathStep::deserializeFromString("World'1")};
    babelwires::Path path(steps);

    EXPECT_EQ(path.getNumSteps(), 4);
    EXPECT_TRUE(path.getStep(0).isField());
    EXPECT_TRUE(path.getStep(1).isIndex());
    EXPECT_TRUE(path.getStep(2).isField());
    EXPECT_TRUE(path.getStep(3).isField());
    EXPECT_EQ(path.getStep(0).asField()->toString(), "Hello");
    EXPECT_EQ(*path.getStep(1).asIndex(), 13);
    EXPECT_EQ(path.getStep(2).asField()->toString(), "Hello");
    EXPECT_EQ(path.getStep(3).asField()->toString(), "World");
    EXPECT_EQ(path.getStep(0).asField()->getDiscriminator(), 2);
    EXPECT_EQ(path.getStep(2).asField()->getDiscriminator(), 3);
    EXPECT_EQ(path.getStep(3).asField()->getDiscriminator(), 1);
}

TEST(FeaturePathTest, pathOps) {
    babelwires::Path path1;

    path1.pushStep(babelwires::PathStep("Forb"));
    path1.pushStep(babelwires::PathStep("Erm"));
    path1.pushStep(babelwires::PathStep(12));
    EXPECT_EQ(path1.getLastStep(), path1.getStep(2));

    babelwires::Path path2 = path1;
    EXPECT_EQ(path1, path2);
    EXPECT_LE(path1, path2);
    EXPECT_TRUE(path1.isPrefixOf(path2));
    EXPECT_FALSE(path1.isStrictPrefixOf(path2));

    EXPECT_EQ(path1.getNumSteps(), 3);
    path1.popStep();
    EXPECT_EQ(path1.getNumSteps(), 2);
    EXPECT_NE(path1, path2);
    EXPECT_LT(path1, path2);
    EXPECT_TRUE(path1.isPrefixOf(path2));
    EXPECT_TRUE(path1.isStrictPrefixOf(path2));
    path2.truncate(2);
    EXPECT_EQ(path1, path2);

    path1.popStep();
    EXPECT_EQ(path1.getNumSteps(), 1);
    path1.popStep();
    EXPECT_EQ(path1.getNumSteps(), 0);

    path2.truncate(0);
    EXPECT_EQ(path2.getNumSteps(), 0);
}

TEST(FeaturePathTest, append) {
    babelwires::Path path0(
        std::vector<babelwires::PathStep>{babelwires::PathStep::deserializeFromString("Hello'2"), 13});
    babelwires::Path path1(
        std::vector<babelwires::PathStep>{babelwires::PathStep::deserializeFromString("Hello'3"),
                                          babelwires::PathStep::deserializeFromString("World'1")});
    babelwires::Path appendedPath = path0;
    appendedPath.append(path1);

    EXPECT_EQ(appendedPath.getNumSteps(), path0.getNumSteps() + path1.getNumSteps());
    for (int i = 0; i < path0.getNumSteps(); ++i) {
        EXPECT_EQ(appendedPath.getStep(i), path0.getStep(i));
    }
    for (int j = 0; j < path1.getNumSteps(); ++j) {
        EXPECT_EQ(appendedPath.getStep(path0.getNumSteps() + j), path1.getStep(j));
    }
}

TEST(FeaturePathTest, removePrefix) {
    babelwires::Path path(
        std::vector<babelwires::PathStep>{babelwires::PathStep::deserializeFromString("Hello'2"), 13,
                                          babelwires::PathStep::deserializeFromString("Hello'3"),
                                          babelwires::PathStep::deserializeFromString("World'1")});
    babelwires::Path path2 = path;
    path2.removePrefix(2);
    EXPECT_EQ(path2.getNumSteps(), 2);
    for (int i = 0; i < path2.getNumSteps(); ++i) {
        EXPECT_EQ(path.getStep(i + 2), path2.getStep(i));
    }
}

TEST(FeaturePathTest, pathIteration) {
    babelwires::Path path1;

    path1.pushStep(babelwires::PathStep("Forb"));
    path1.pushStep(babelwires::PathStep("Erm"));
    path1.pushStep(babelwires::PathStep(12));

    babelwires::Path path2;
    for (const auto& s : path1) {
        path2.pushStep(s);
    }
    EXPECT_EQ(path2.getNumSteps(), 3);
    EXPECT_EQ(path2, path1);
}


TEST(FeaturePathTest, pathFollow) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot testRecordFeature(testEnvironment.m_typeSystem,
                                                     testUtils::TestComplexRecordType::getThisIdentifier());
    testRecordFeature.setToDefault();

    testUtils::TestComplexRecordTypeFeatureInfo info(testRecordFeature);

    EXPECT_EQ(&info.m_pathToRecord.follow(testRecordFeature), &testRecordFeature);
    EXPECT_EQ(&info.m_pathToInt.follow(testRecordFeature), &info.m_intFeature);
    EXPECT_EQ(&info.m_pathToArray.follow(testRecordFeature), &info.m_arrayFeature);
    EXPECT_EQ(&info.m_pathToElem0.follow(testRecordFeature), &info.m_elem0);
    EXPECT_EQ(&info.m_pathToElem1.follow(testRecordFeature), &info.m_elem1);
    EXPECT_EQ(&info.m_pathToSubRecord.follow(testRecordFeature), &info.m_subRecordFeature);
    EXPECT_EQ(&info.m_pathToSubRecordInt.follow(testRecordFeature), &info.m_subRecordIntFeature);

    EXPECT_EQ(info.m_pathToRecord.getNumSteps(), 0);

    EXPECT_EQ(info.m_pathToInt.getNumSteps(), 1);
    EXPECT_EQ(info.m_pathToInt.getStep(0).getField(), testUtils::TestComplexRecordType::getInt0Id());

    EXPECT_EQ(info.m_pathToArray.getNumSteps(), 1);
    EXPECT_EQ(info.m_pathToArray.getStep(0).getField(), testUtils::TestComplexRecordType::getArrayId());

    EXPECT_EQ(info.m_pathToElem0.getNumSteps(), 2);
    EXPECT_EQ(info.m_pathToElem0.getStep(0).getField(), testUtils::TestComplexRecordType::getArrayId());
    EXPECT_EQ(info.m_pathToElem0.getStep(1).getIndex(), 0);

    EXPECT_EQ(info.m_pathToElem1.getNumSteps(), 2);
    EXPECT_EQ(info.m_pathToElem1.getStep(0).getField(), testUtils::TestComplexRecordType::getArrayId());
    EXPECT_EQ(info.m_pathToElem1.getStep(1).getIndex(), 1);

    EXPECT_EQ(info.m_pathToSubRecord.getNumSteps(), 1);
    EXPECT_EQ(info.m_pathToSubRecord.getStep(0).getField(), testUtils::TestComplexRecordType::getSubrecordId());

    EXPECT_EQ(info.m_pathToSubRecordInt.getNumSteps(), 2);
    EXPECT_EQ(info.m_pathToSubRecordInt.getStep(0).getField(), testUtils::TestComplexRecordType::getSubrecordId());
    EXPECT_EQ(info.m_pathToSubRecordInt.getStep(1).getField(), testUtils::TestSimpleRecordType::getInt0Id());
}

TEST(FeaturePathTest, pathResolve) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::Path pathToInt;
    babelwires::Path pathToArray;
    babelwires::Path pathToElem0;
    babelwires::Path pathToElem1;
    babelwires::Path pathToSubRecord;
    babelwires::Path pathToInt2;

    pathToInt.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::s_intIdInitializer));
    pathToArray.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::s_arrayIdInitializer));
    pathToElem0.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::s_arrayIdInitializer));
    pathToElem0.pushStep(babelwires::PathStep(0));
    pathToElem1.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::s_arrayIdInitializer));
    pathToElem1.pushStep(babelwires::PathStep(1));
    pathToSubRecord.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::s_subRecordIdInitializer));
    pathToInt2.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::s_subRecordIdInitializer));
    pathToInt2.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::s_int0IdInitializer));

    EXPECT_EQ(pathToInt.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToArray.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToElem0.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToElem1.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToSubRecord.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToInt2.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToInt2.getStep(1).getField().getDiscriminator(), 0);

    babelwires::ValueTreeRoot testRecordFeature(testEnvironment.m_typeSystem,
                                                     testUtils::TestComplexRecordType::getThisIdentifier());
    testRecordFeature.setToDefault();

    testUtils::TestComplexRecordTypeFeatureInfo info(testRecordFeature);

    // The paths should work even though they didn't use registered field identifiers.
    EXPECT_EQ(&pathToInt.follow(testRecordFeature), &info.m_intFeature);
    EXPECT_EQ(&pathToArray.follow(testRecordFeature), &info.m_arrayFeature);
    EXPECT_EQ(&pathToElem0.follow(testRecordFeature), &info.m_elem0);
    EXPECT_EQ(&pathToElem1.follow(testRecordFeature), &info.m_elem1);
    EXPECT_EQ(&pathToSubRecord.follow(testRecordFeature), &info.m_subRecordFeature);
    EXPECT_EQ(&pathToInt2.follow(testRecordFeature), &info.m_subRecordIntFeature);

    // The field identifiers in the paths should now be resolved.
    EXPECT_NE(pathToInt.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToArray.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToElem0.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToElem1.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToSubRecord.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToInt2.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_NE(pathToInt2.getStep(1).getField().getDiscriminator(), 0);

    // And they should be resolved to the values in the registered identifiers.
    EXPECT_EQ(pathToInt.getStep(0).getField().getDiscriminator(), testUtils::TestComplexRecordType::getInt0Id().getDiscriminator());
    EXPECT_EQ(pathToArray.getStep(0).getField().getDiscriminator(), testUtils::TestComplexRecordType::getArrayId().getDiscriminator());
    EXPECT_EQ(pathToElem0.getStep(0).getField().getDiscriminator(), testUtils::TestComplexRecordType::getArrayId().getDiscriminator());
    EXPECT_EQ(pathToElem1.getStep(0).getField().getDiscriminator(), testUtils::TestComplexRecordType::getArrayId().getDiscriminator());
    EXPECT_EQ(pathToSubRecord.getStep(0).getField().getDiscriminator(),
              testUtils::TestComplexRecordType::getSubrecordId().getDiscriminator());
    EXPECT_EQ(pathToInt2.getStep(0).getField().getDiscriminator(), testUtils::TestComplexRecordType::getSubrecordId().getDiscriminator());
    EXPECT_EQ(pathToInt2.getStep(1).getField().getDiscriminator(), testUtils::TestSimpleRecordType::getInt0Id().getDiscriminator());
}

TEST(FeaturePathTest, pathTryFollow) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot testRecordFeature(testEnvironment.m_typeSystem,
                                                     testUtils::TestComplexRecordType::getThisIdentifier());
    testRecordFeature.setToDefault();

    testUtils::TestComplexRecordTypeFeatureInfo info(testRecordFeature);

    EXPECT_EQ(info.m_pathToRecord.tryFollow(testRecordFeature), &testRecordFeature);
    EXPECT_EQ(info.m_pathToInt.tryFollow(testRecordFeature), &info.m_intFeature);
    EXPECT_EQ(info.m_pathToArray.tryFollow(testRecordFeature), &info.m_arrayFeature);
    EXPECT_EQ(info.m_pathToElem0.tryFollow(testRecordFeature), &info.m_elem0);
    EXPECT_EQ(info.m_pathToElem1.tryFollow(testRecordFeature), &info.m_elem1);
    EXPECT_EQ(info.m_pathToSubRecord.tryFollow(testRecordFeature), &info.m_subRecordFeature);
    EXPECT_EQ(info.m_pathToSubRecordInt.tryFollow(testRecordFeature), &info.m_subRecordIntFeature);
}

TEST(FeaturePathTest, pathFollowFail) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::Path pathToNonField;
    babelwires::Path pathToNonIndex;
    babelwires::Path pathOffEndOfArray;
    babelwires::Path pathValueAsRecord;
    babelwires::Path pathValueAsArray;

    pathToNonField.pushStep(babelwires::PathStep("Forb"));
    pathToNonIndex.pushStep(babelwires::PathStep(0));
    pathOffEndOfArray.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::getArrayId()));
    pathOffEndOfArray.pushStep(babelwires::PathStep(5));
    pathValueAsRecord.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::getSubrecordId()));
    pathValueAsRecord.pushStep(babelwires::PathStep("flerm"));
    pathValueAsArray.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::getInt1Id()));
    pathValueAsArray.pushStep(babelwires::PathStep(12));

    babelwires::ValueTreeRoot testRecordFeature(testEnvironment.m_typeSystem,
                                                     testUtils::TestComplexRecordType::getThisIdentifier());
    testRecordFeature.setToDefault();

    testUtils::TestComplexRecordTypeFeatureInfo info(testRecordFeature);

    EXPECT_THROW(pathToNonField.follow(testRecordFeature), babelwires::ModelException);
    EXPECT_THROW(pathToNonIndex.follow(testRecordFeature), babelwires::ModelException);
    EXPECT_THROW(pathOffEndOfArray.follow(testRecordFeature), babelwires::ModelException);
    EXPECT_THROW(pathValueAsRecord.follow(testRecordFeature), babelwires::ModelException);
    EXPECT_THROW(pathValueAsArray.follow(testRecordFeature), babelwires::ModelException);

    EXPECT_EQ(pathToNonField.tryFollow(testRecordFeature), nullptr);
    EXPECT_EQ(pathToNonIndex.tryFollow(testRecordFeature), nullptr);
    EXPECT_EQ(pathOffEndOfArray.tryFollow(testRecordFeature), nullptr);
    EXPECT_EQ(pathValueAsRecord.tryFollow(testRecordFeature), nullptr);
    EXPECT_EQ(pathValueAsArray.tryFollow(testRecordFeature), nullptr);
}

TEST(FeaturePathTest, pathSerialization) {
    babelwires::Path path;

    EXPECT_EQ(path.serializeToString(), "");

    path.pushStep(babelwires::PathStep("Forb"));
    path.pushStep(babelwires::PathStep("Erm"));
    path.pushStep(babelwires::PathStep(12));

    EXPECT_EQ(path.serializeToString(), "Forb/Erm/12");

    path.getStep(0).getField().setDiscriminator(2);
    path.getStep(1).getField().setDiscriminator(4);

    EXPECT_EQ(path.serializeToString(), "Forb'2/Erm'4/12");
}

TEST(FeaturePathTest, pathDeserialization) {
    EXPECT_EQ(babelwires::Path::deserializeFromString("").getNumSteps(), 0);

    babelwires::Path path1;
    EXPECT_NO_THROW(path1 = babelwires::Path::deserializeFromString("Forb/12/Erm"));
    EXPECT_EQ(path1.getNumSteps(), 3);
    EXPECT_EQ(path1.getStep(0), babelwires::PathStep("Forb"));
    EXPECT_EQ(path1.getStep(1), babelwires::PathStep(12));
    EXPECT_EQ(path1.getStep(2), babelwires::PathStep("Erm"));

    babelwires::Path path2;
    EXPECT_NO_THROW(path2 = babelwires::Path::deserializeFromString("12/Forb/Erm"));
    EXPECT_EQ(path2.getNumSteps(), 3);
    EXPECT_EQ(path2.getStep(0), babelwires::PathStep(12));
    EXPECT_EQ(path2.getStep(1), babelwires::PathStep("Forb"));
    EXPECT_EQ(path2.getStep(2), babelwires::PathStep("Erm"));

    babelwires::Path path3;
    EXPECT_NO_THROW(path3 = babelwires::Path::deserializeFromString("Forb'2/12/Erm'4"));
    EXPECT_EQ(path3.getNumSteps(), 3);
    EXPECT_EQ(path3.getStep(0), babelwires::PathStep("Forb"));
    EXPECT_EQ(path3.getStep(0).getField().getDiscriminator(), 2);
    EXPECT_EQ(path3.getStep(1), babelwires::PathStep(12));
    EXPECT_EQ(path3.getStep(2), babelwires::PathStep("Erm"));
    EXPECT_EQ(path3.getStep(2).getField().getDiscriminator(), 4);

    EXPECT_THROW(babelwires::Path::deserializeFromString("Foo//Bar"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Path::deserializeFromString("/Foo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Path::deserializeFromString("'23/Foo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Path::deserializeFromString("Foo/Hællo/Foo"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Path::deserializeFromString("Foo/3Hello"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Path::deserializeFromString("Foo/Erm'233232"), babelwires::ParseException);
    EXPECT_THROW(babelwires::Path::deserializeFromString("Foo/Erm/"), babelwires::ParseException);
}

TEST(FeaturePathTest, pathHash) {
    babelwires::Path path;
    const std::size_t hashWhenEmpty = path.getHash();

    path.pushStep(0);
    const std::size_t hashWithZero = path.getHash();

    path.popStep();
    const std::size_t hashWhenEmptyAgain = path.getHash();

    path.pushStep(1);
    const std::size_t hashWithOne = path.getHash();

    path.pushStep(babelwires::PathStep("Hello"));
    const std::size_t hashWithOneHello = path.getHash();

    path.popStep();
    const std::size_t hashWithOneAgain = path.getHash();
    path.pushStep(babelwires::PathStep("Byebye"));
    const std::size_t hashWithOneByebye = path.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_EQ(hashWhenEmpty, hashWhenEmptyAgain);
    EXPECT_NE(hashWhenEmpty, hashWithZero);
    EXPECT_NE(hashWithZero, hashWithOne);
    EXPECT_NE(hashWithOne, hashWithOneHello);
    EXPECT_EQ(hashWithOne, hashWithOneAgain);
    EXPECT_NE(hashWithOneHello, hashWithOneByebye);
}
