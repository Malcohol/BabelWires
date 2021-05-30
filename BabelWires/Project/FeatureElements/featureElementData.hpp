/**
 * FeatureElementDatas carry the data sufficient to reconstruct a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/projectIds.hpp"
#include "BabelWires/Project/uiPosition.hpp"

#include "Common/Cloning/cloneable.hpp"
#include "Common/Serialization/serializable.hpp"
#include "Common/types.hpp"
#include "Common/Log/userLogger.hpp"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

namespace babelwires {

    class UserLogger;
    class ProjectContext;
    class FeatureElement;
    class UntypedRegistry;

    /// Use the custom clone framework to do a shallow clone.
    struct ShallowCloneContext {};

    /// Data of interest only to the UI, and has no bearing on the evaluation logic.
    /// Note: The expand/collapse logic could have been included here, but I decided to pack
    /// it into one EditTree with the modifiers.
    struct UiData : Serializable {
        SERIALIZABLE(UiData, "uiData", void, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// The position this element is at in the UI.
        UiPosition m_uiPosition;

        /// The size of the element in the UI.
        UiSize m_uiSize;
    };

    /// FeatureElementDatas carry the data sufficient to reconstruct a FeatureElement.
    /// The class supports clone(), which returns a deep clone of the object,
    /// and customClone() which returns a clone with the array members unset.
    struct ElementData : Cloneable, CustomCloneable<ShallowCloneContext>, Serializable, ProjectVisitable {
        CLONEABLE_ABSTRACT(ElementData);
        CUSTOM_CLONEABLE_ABSTRACT(ElementData);
        SERIALIZABLE_ABSTRACT(ElementData, "ElementData", void);

        ElementData() = default;

        /// The copy constructor constructs an array of _clones_ of the modifiers.
        ElementData(const ElementData& other);

        /// Like the copy constructor, but don't copy the modifiers and expanded paths.
        ElementData(const ElementData& other, ShallowCloneContext);

        /// Unless m_factoryVersion is 0 (which means "ignore versions"), this should return false
        /// if the m_factoryVersion is different than the factory version registered in the context.
        /// Warn if it is less and error if it is more.
        virtual bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) = 0;

        std::unique_ptr<FeatureElement> createFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                             ElementId newId) const;

        /// Call the visitor on all the FieldIdentifiers in the element.
        void visitFields(FieldVisitor& visitor) override;

        /// Call the visitor on all the FilePaths in the element.
        /// This base implementation does nothing.
        void visitFilePaths(FilePathVisitor& visitor) override;

        /// The factory which creates FeatureElements corresponding to this data.
        std::string m_factoryIdentifier;

        /// The version of the factory to which these contents relate.
        /// 0 is treated specially, and means that versioning should be ignored, and the data should
        /// be considered implicitly up-to-date.
        VersionNumber m_factoryVersion = 0;

        /// Uniquely identifies the Element within the project.
        ElementId m_id = INVALID_ELEMENT_ID;

        /// Data solely relevant to the UI.
        UiData m_uiData;

        /// The set of modifiers applied to this modifier.
        std::vector<std::unique_ptr<ModifierData>> m_modifiers;

        /// Those paths within the feature which are expanded.
        /// Note: Only those which are present in the features are maintained on serialization.
        std::vector<FeaturePath> m_expandedPaths;

      protected:
        virtual std::unique_ptr<FeatureElement>
        doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger, ElementId newId) const = 0;

        /// For use when serializing subclasses.
        void addCommonKeyValuePairs(Serializer& serializer) const;
        void getCommonKeyValuePairs(Deserializer& deserializer);

        /// For use when serializing subclasses.
        void serializeModifiers(Serializer& serializer) const;
        void deserializeModifiers(Deserializer& serializer);

        /// For use when serializing subclasses.
        void serializeUiData(Serializer& serializer) const;
        void deserializeUiData(Deserializer& deserializer);

        /// Look up the ID in the reg, and issue a warning if there are version incompatibilities.
        template<typename REGISTRY>
        bool checkFactoryVersionCommon(const REGISTRY& reg, UserLogger& userLogger,
                                   const std::string factoryIdentifier, VersionNumber& thisVersion);
    };
} // namespace babelwires

#include "BabelWires/Project/FeatureElements/featureElementData_inl.hpp"
