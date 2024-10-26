/**
 * ValueElements are FeatureElements which carry a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>

babelwires::ValueElement::ValueElement(const ProjectContext& context, UserLogger& userLogger,
                                       const ValueElementData& data, ElementId newId)
    : FeatureElement(data, newId) {
    setFactoryName(data.getTypeRef().toString());
    TypeRef typeRefForConstruction = data.getTypeRef();
    if (!typeRefForConstruction.tryResolve(context.m_typeSystem)) {
        typeRefForConstruction = FailureType::getThisIdentifier();
        std::ostringstream message;
        message << "Type Reference " << data.getTypeRef().toString() << " could not be resolved";
        setInternalFailure(message.str());
    }
    m_rootFeature = std::make_unique<ValueTreeRoot>(context.m_typeSystem, typeRefForConstruction);
}

babelwires::ValueElement::~ValueElement() = default;

const babelwires::ValueElementData& babelwires::ValueElement::getElementData() const {
    return static_cast<const ValueElementData&>(FeatureElement::getElementData());
}

babelwires::ValueTreeNode* babelwires::ValueElement::doGetInputFeatureNonConst() {
    return m_rootFeature.get();
}

babelwires::ValueTreeNode* babelwires::ValueElement::doGetOutputFeatureNonConst() {
    return m_rootFeature.get();
}

const babelwires::ValueTreeNode* babelwires::ValueElement::getInputFeature() const {
    return m_rootFeature.get();
}

const babelwires::ValueTreeNode* babelwires::ValueElement::getOutputFeature() const {
    return m_rootFeature.get();
}

std::string babelwires::ValueElement::getRootLabel() const {
    if (m_rootFeature->getTypeRef() == FailureType::getThisIdentifier()) {
        return "Failed";
    } else {
        return "Value";
    }
}

void babelwires::ValueElement::doProcess(UserLogger& userLogger) {
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        m_contentsCache.setFeatures(getRootLabel(), m_rootFeature.get(), m_rootFeature.get());
    } else if (isChanged(Changes::ModifierChangesMask)) {
        m_contentsCache.updateModifierCache();
    }
}
