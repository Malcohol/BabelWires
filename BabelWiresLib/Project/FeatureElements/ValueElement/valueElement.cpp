/**
 * ValueElements are FeatureElements which carry a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::ValueElement::ValueElement(const ProjectContext& context, UserLogger& userLogger,
                                       const ValueElementData& data, ElementId newId)
    : FeatureElement(data, newId) {
    m_rootFeature = std::make_unique<RootFeature>(context);
    const TypeRef& typeRef = data.getTypeRef();
    setFactoryName(typeRef.toString());
    const Type* const type = typeRef.tryResolve(context.m_typeSystem);
    if (type) {
        m_rootFeature->addField(std::make_unique<SimpleValueFeature>(typeRef), getStepToValue());
    } else {
        std::ostringstream message;
        message << "Type Reference " << typeRef << " could not be resolved";
        setInternalFailure(message.str());
        userLogger.logError() << "Failed to create ValueElement id=" << newId << ": " << message.str();
    }
}

babelwires::ValueElement::~ValueElement() = default;

babelwires::ShortId babelwires::ValueElement::getStepToValue() {
    return BW_SHORT_ID("value", "value", "46b077cb-da6a-4729-92ac-091e9995db7f");
}

const babelwires::ValueElementData& babelwires::ValueElement::getElementData() const {
    return static_cast<const ValueElementData&>(FeatureElement::getElementData());
}

babelwires::Feature* babelwires::ValueElement::doGetInputFeatureNonConst() {
    return m_rootFeature.get();
}

babelwires::Feature* babelwires::ValueElement::doGetOutputFeatureNonConst() {
    return m_rootFeature.get();
}

const babelwires::Feature* babelwires::ValueElement::getInputFeature() const {
    return m_rootFeature.get();
}

const babelwires::Feature* babelwires::ValueElement::getOutputFeature() const {
    return m_rootFeature.get();
}

void babelwires::ValueElement::doProcess(UserLogger& userLogger) {
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        m_contentsCache.setFeatures(m_rootFeature.get(), m_rootFeature.get());
    } else if (isChanged(Changes::ModifierChangesMask)) {
        m_contentsCache.updateModifierCache();
    }
}
