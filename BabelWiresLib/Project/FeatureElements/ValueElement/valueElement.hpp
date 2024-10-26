/**
 * ValueElements are FeatureElements which carry a value.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    struct UserLogger;
    class ValueElementData;
    class SimpleValueFeature;

    class ValueElement : public FeatureElement {
      public:
        ValueElement(const ProjectContext& context, UserLogger& userLogger, const ValueElementData& data,
                         ElementId newId);
        ~ValueElement();

        /// Down-cast version of the parent's method.
        const ValueElementData& getElementData() const;

        virtual const ValueTreeNode* getInputFeature() const override;
        virtual const ValueTreeNode* getOutputFeature() const override;

      protected:
        ValueTreeNode* doGetInputFeatureNonConst() override;
        ValueTreeNode* doGetOutputFeatureNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        std::string getRootLabel() const;

      private:
        std::unique_ptr<babelwires::SimpleValueFeature> m_rootFeature;
    };

} // namespace babelwires
