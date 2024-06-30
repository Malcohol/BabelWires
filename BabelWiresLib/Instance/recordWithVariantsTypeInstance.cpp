/**
 * 
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Instance/recordWithVariantsTypeInstance.hpp>

#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>

babelwires::ShortId babelwires::RecordWithVariantsInstanceUtil::getSelectedTag(const ValueFeature& valueFeature) {
    const RecordWithVariantsType& type = valueFeature.getType().is<RecordWithVariantsType>();
    const ValueHolder& value = valueFeature.getValue();
    return type.getSelectedTag(value);
}

void babelwires::RecordWithVariantsInstanceUtil::selectTag(ValueFeature& valueFeature, ShortId tag) {
    const RecordWithVariantsType& type = valueFeature.getType().is<RecordWithVariantsType>();
    ValueHolder value = valueFeature.getValue();
    if (tag != type.getSelectedTag(value)) {
        const babelwires::TypeSystem& typeSystem = babelwires::RootFeature::getTypeSystemAt(valueFeature);
        type.selectTag(typeSystem, value, tag);
        valueFeature.setValue(value);
    }
}
