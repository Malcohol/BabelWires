/**
 * A RecordFeature carries a sequence of features identified by FeatureIdentifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename T> T* babelwires::RecordFeature::addField(std::unique_ptr<T> f, const FieldIdentifier& identifier) {
    T* fTPtr = f.get();
    addFieldInternal(std::move(f), identifier);
    return fTPtr;
}
