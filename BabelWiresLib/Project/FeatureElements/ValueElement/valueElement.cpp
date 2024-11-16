/**
 * ValueElements are FeatureElements which carry a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
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
        typeRefForConstruction = FailureType::getThisType();
        std::ostringstream message;
        message << "Type Reference " << data.getTypeRef().toString() << " could not be resolved";
        setInternalFailure(message.str());
    }
    m_valueTreeRoot = std::make_unique<ValueTreeRoot>(context.m_typeSystem, typeRefForConstruction);
}

babelwires::ValueElement::~ValueElement() = default;

const babelwires::ValueElementData& babelwires::ValueElement::getElementData() const {
    return static_cast<const ValueElementData&>(FeatureElement::getElementData());
}

babelwires::ValueTreeNode* babelwires::ValueElement::doGetInputNonConst() {
    return m_valueTreeRoot.get();
}

babelwires::ValueTreeNode* babelwires::ValueElement::doGetOutputNonConst() {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* babelwires::ValueElement::getInput() const {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* babelwires::ValueElement::getOutput() const {
    return m_valueTreeRoot.get();
}

std::string babelwires::ValueElement::getRootLabel() const {
    if (m_valueTreeRoot->getTypeRef() == FailureType::getThisType()) {
        return "Failed";
    } else {
        return "Value";
    }
}

void babelwires::ValueElement::doProcess(UserLogger& userLogger) {
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        setValueTrees(getRootLabel(), m_valueTreeRoot.get(), m_valueTreeRoot.get());
    } else if (isChanged(Changes::ModifierChangesMask)) {
        updateModifierCache();
    }
}
