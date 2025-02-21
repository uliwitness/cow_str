// Copyright 2025 Uli Kusterer.
// Licensed under the zlib license.

#pragma once

#include <cstring>
#include <cstdlib>
#include <ostream>
#include <atomic>

namespace cow {
	
	using namespace std;
	
	/*! buf is a refcounted string storage. It always allocates a nul terminator for cheap
		conversion to C-string. The refcount is guaranteed to be thread-safe, for write
		access you must be exclusive owner of a buf, or you will not be thread-safe. */
	class buf {
	public:
		enum is_constant {
			not_constant,
			constant
		};
		
		buf() : _bytes{&empty_cstr}, _len{0}, _isconstant{true} {}
		buf(const char* str, is_constant isc) : _len(strlen(str)), _isconstant{isc == constant} { if (_isconstant) { _bytes = const_cast<char*>(str); } else { _bytes = (char*) calloc(_len + 1, 1); memcpy(_bytes, str, _len); } }
		buf(const char* str, size_t len) : _bytes((char*)calloc(len + 1, 1)), _len(len) { memcpy(_bytes, str, _len); }
		buf(size_t count, char ch) : _bytes((char*)calloc(count + 1, 1)), _len(count) { memset(_bytes, ch, _len); }
		buf(const buf&) = delete;
		
		~buf() {
			if (!_isconstant) {
				free(_bytes);
			}
			_bytes = nullptr;
			_isconstant = true;
			_len = 0;
		}
		
		/*! Copy buf if caller is not the only owner.
			Gives up ownership of buf if it gives you a new copy. */
		buf* mutableCopy() {
			if (_refcount == 1) {
				return this;
			} else {
				auto newBuf = new buf(_bytes, _len);
				release();
				return newBuf;
			}
		}
		//! Increment refcount to make us co-owner of buf.
		buf* retain() { ++_refcount; return this; };
		//! Give up caller's ownership of buf, deleting it of caller was last owner.
		void release() { if (--_refcount == 0) { delete this; } }

		const char* data() const { return _bytes; };
		char* data() { return _bytes; };
		
		size_t length() { return _len; }
		
		static buf* empty_buf() { return _empty_buf.retain(); }
		
	private:
		static char empty_cstr;
		static buf _empty_buf;
		
		char *_bytes{nullptr};
		size_t _len : 63{0};
		size_t _isconstant : 1{false};
		atomic<size_t> _refcount{1};
	};
	static_assert(sizeof(buf) == sizeof(char*) + sizeof(size_t) + sizeof(size_t), "Uses minimal space");
	
	/*! str guarantees:
		- _buf is lazily copied on write.
		- only calls that mutate _buf invalidate iterators. (non-const [] and at() are considered mutation)
		- default constructed strings do not incur an additional buf allocation.
		- constant strings constructed with ""_cow will not copy their string until actually mutated.
		- limited thread-safety: copies of strings can be moved between threads as they will only read-access their buf (and will copy it on write access)
	 */
	class str {
	public:
		using is_constant = buf::is_constant;
		using const_iterator = const char*;
		using iterator = char*;

		str() { _buf = buf::empty_buf(); }
		str(const char* str, is_constant isc = is_constant::not_constant) : _buf(new buf(str, isc)) {}
		str(const char* str, size_t len) : _buf(new buf(str, len)) {}
		str(const str& inOriginal) : _buf(inOriginal._buf->retain()) {}
		str(str&& inOriginal) : _buf(inOriginal._buf) { inOriginal._buf = nullptr; }
		~str() { _buf->release(); }
		
		size_t length() const { return _buf->length(); }
		size_t size() const { return _buf->length(); }
		bool empty() const { return _buf->length() == 0; }

		const_iterator begin() const { return _buf->data(); }
		const_iterator end() const { return _buf->data() + _buf->length(); }
		
		iterator begin() { return _buf->data(); }
		iterator end() { return _buf->data() + _buf->length(); }

		char operator [] (size_t idx) const { return _buf->data()[idx]; }
		char& operator [] (size_t idx) { _buf = _buf->mutableCopy(); return _buf->data()[idx]; }

		char at(size_t idx) const { if (idx >= _buf->length()) { throw range_error("cow::str::at"); } return _buf->data()[idx]; }
		char& at(size_t idx) { if (idx >= _buf->length()) { throw range_error("cow::str::at"); } _buf = _buf->mutableCopy(); return _buf->data()[idx]; }

		const char* c_str() const { return _buf->data(); }
		const char* data() const { return _buf->data(); }
		char* data() { _buf = _buf->mutableCopy(); return _buf->data(); }

		void append(const str& inNewEnd);
		
		//void insert(iterator dest, str& inNewEnd);

	private:
		buf *_buf;
	};

} // namespace cow

cow::str operator ""_cow (const char* inLiteralStr, unsigned long);

//! So you can use cow::str with std::cout.
extern std::ostream& operator << (std::ostream& inStream, const cow::str& inStr);
