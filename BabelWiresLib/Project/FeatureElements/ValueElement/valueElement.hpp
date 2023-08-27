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
    class RootFeature;
    class ValueElementData;

    class ValueElement : public FeatureElement {
      public:
        ValueElement(const ProjectContext& context, UserLogger& userLogger, const ValueElementData& data,
                         ElementId newId);
        ~ValueElement();

        /// Down-cast version of the parent's method.
        const ValueElementData& getElementData() const;

        virtual RootFeature* getInputFeatureNonConst() override;
        virtual RootFeature* getOutputFeatureNonConst() override;
        virtual const RootFeature* getInputFeature() const override;
        virtual const RootFeature* getOutputFeature() const override;

      protected:
        void doProcess(UserLogger& userLogger) override;

      private:
        std::unique_ptr<babelwires::RootFeature> m_rootFeature;
    };

} // namespace babelwires
