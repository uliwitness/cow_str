// Copyright 2025 Uli Kusterer.
// Licensed under the zlib license.

#include "cow_str.hpp"

using namespace cow;
using namespace std;

char buf::empty_cstr{'\0'};
buf buf::_empty_buf;


cow::str operator ""_cow (const char* inLiteralStr, unsigned long) {
	return str(inLiteralStr, str::is_constant::constant);
}

std::ostream& operator << (std::ostream& inStream, const cow::str& inStr) {
	inStream << inStr.c_str();
	return inStream;
}

void str::append(const str& inNewEnd) {
	if (inNewEnd.empty()) { return; }
	auto oldBuf = _buf;
	_buf = new buf(_buf->length() + inNewEnd.length(), '\0');
	memcpy(_buf->data(), oldBuf->data(), oldBuf->length());
	memcpy(_buf->data() + oldBuf->length(), inNewEnd._buf->data(), inNewEnd.length());
	oldBuf->release();
}

void str::insert(iterator dest, const str& inNewEnd) {
	if (inNewEnd.empty()) { return; }
	size_t offset = distance(_buf->data(), dest);
	auto oldBuf = _buf;
	_buf = new buf(_buf->length() + inNewEnd.length(), '\0');
	if (offset > 0) {
		memcpy(_buf->data(), oldBuf->data(), offset);
	}
	memcpy(_buf->data() + offset, inNewEnd.begin(), inNewEnd.length());
	size_t afterInsertionOffset = offset + inNewEnd.length();
	if (afterInsertionOffset < _buf->length()) {
		memcpy(_buf->data() + afterInsertionOffset, oldBuf->data() + offset, oldBuf->length() - offset);
	}
	oldBuf->release();
}
