#pragma once

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Features/recordFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>

namespace testUtils {
    /// A record with a small feature hierarchy.
    struct TestRootFeature : babelwires::RootFeature {
        // Contained ints have an imposed value limit.
        // This is used to test failed applications.
        TestRootFeature(const babelwires::ProjectContext& context, int intValueLimit = 255, bool addExtraInt = false);

        static constexpr char s_intIdInitializer[] = "int0";
        static constexpr char s_recordIdInitializer[] = "record";
        // Deliberately match the field in TestFileFeature.
        static constexpr char s_int2IdInitializer[] = "aaa";
        static constexpr char s_extraIntIdInitializer[] = "extra";

        static constexpr char s_intFieldName[] = "the int";
        static constexpr char s_recordFieldName[] = "the record";
        static constexpr char s_int2FieldName[] = "the other int";
        static constexpr char s_extraIntFieldName[] = "the extra int";

        static constexpr char s_intUuid[] = "00000000-1111-2222-3333-800001000001";
        static constexpr char s_recordUuid[] = "00000000-1111-2222-3333-800001000003";
        static constexpr char s_int2Uuid[] = "00000000-1111-2222-3333-800000100004";
        static constexpr char s_extraIntUuid[] = "00000000-1111-2222-3333-800001000005";

        babelwires::ShortId m_intId;
        babelwires::ShortId m_recordId;
        babelwires::ShortId m_int2Id;
        babelwires::ShortId m_extraIntId;

        babelwires::IntFeature* m_intFeature;
        babelwires::RecordFeature* m_subRecordFeature;
        babelwires::IntFeature* m_intFeature2;
        /// if addExtraInt is true, then there's another int feature at "record/aaa"
        babelwires::IntFeature* m_extraIntFeature = nullptr;

        // Contained ints have an imposed value limit.
        // This is used to test failed applications.
        int m_intValueLimit;

        // For convenience
        static const babelwires::FeaturePath s_pathToInt;
        static const babelwires::FeaturePath s_pathToRecord;
        static const babelwires::FeaturePath s_pathToInt2;
        static const babelwires::FeaturePath s_pathToExtraInt;
    };
} // namespace testUtils
