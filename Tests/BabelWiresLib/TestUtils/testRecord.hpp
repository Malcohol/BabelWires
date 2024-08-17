#pragma once

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/recordFeature.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>

namespace testUtils {
    /// A record with a small feature hierarchy.
    struct TestRecordFeature : babelwires::RecordFeature {
        // Contained ints have an imposed value limit.
        // This is used to test failed applications.
        TestRecordFeature(int intValueLimit = 255, bool addExtraInt = false);

        void doSetToDefault() override;

        static constexpr char s_intIdInitializer[] = "int0";
        static constexpr char s_arrayIdInitializer[] = "array";
        static constexpr char s_recordIdInitializer[] = "record";
        // Deliberately match the field in TestFileFeature.
        static constexpr char s_int2IdInitializer[] = "aaa";
        static constexpr char s_extraIntIdInitializer[] = "extra";

        static constexpr char s_intFieldName[] = "the int";
        static constexpr char s_arrayFieldName[] = "the array";
        static constexpr char s_recordFieldName[] = "the record";
        static constexpr char s_int2FieldName[] = "the other int";
        static constexpr char s_extraIntFieldName[] = "the extra int";

        static constexpr char s_intUuid[] = "00000000-1111-2222-3333-800000000001";
        static constexpr char s_arrayUuid[] = "00000000-1111-2222-3333-800000000002";
        static constexpr char s_recordUuid[] = "00000000-1111-2222-3333-800000000003";
        static constexpr char s_int2Uuid[] = "00000000-1111-2222-3333-800000000004";
        static constexpr char s_extraIntUuid[] = "00000000-1111-2222-3333-800000000005";

        babelwires::ShortId m_intId;
        babelwires::ShortId m_arrayId;
        babelwires::ShortId m_recordId;
        babelwires::ShortId m_int2Id;
        babelwires::ShortId m_extraIntId;

        babelwires::IntFeature* m_intFeature;
        // This has a min and default size of 2.
        babelwires::ArrayFeature* m_arrayFeature;
        babelwires::IntFeature* m_elem0;
        babelwires::IntFeature* m_elem1;
        babelwires::RecordFeature* m_subRecordFeature;
        babelwires::IntFeature* m_intFeature2;
        /// if addExtraInt is true, then there's another int feature at "record/extra"
        babelwires::IntFeature* m_extraIntFeature = nullptr;

        // Contained ints have an imposed value limit.
        // This is used to test failed applications.
        int m_intValueLimit;
    };
} // namespace testUtils
