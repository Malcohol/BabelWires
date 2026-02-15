/**
 * A DataSink provides a destination for bytes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/IO/dataSink.hpp>

babelwires::DataSink::DataSink(DataSink&& other) noexcept
	: m_hasBeenClosed(other.m_hasBeenClosed) {
	other.m_hasBeenClosed = true;
}

babelwires::DataSink& babelwires::DataSink::operator=(DataSink&& other) noexcept {
	if (this != &other) {
		m_hasBeenClosed = other.m_hasBeenClosed;
		other.m_hasBeenClosed = true;
	}
	return *this;
}

babelwires::DataSink::~DataSink() {
	assert(m_hasBeenClosed && "Close must be called on the DataSink before it is destroyed");
}

babelwires::Result babelwires::DataSink::close(ErrorState errorState) {
	m_hasBeenClosed = true;
	return doClose(errorState);
}
