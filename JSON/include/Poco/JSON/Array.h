//
// Array.h
//
// Library: JSON
// Package: JSON
// Module:  Array
//
// Definition of the Array class.
//
// Copyright (c) 2012, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef JSON_Array_INCLUDED
#define JSON_Array_INCLUDED


#include "Poco/JSON/JSON.h"
#include "Poco/SharedPtr.h"
#include "Poco/Dynamic/Var.h"
#include <vector>
#include <sstream>


namespace Poco {

namespace JSON {

class JSON_API Array;

}


#if defined(POCO_OS_FAMILY_WINDOWS)
// Explicitly instantiated shared pointer in JSON library
extern template class Poco::SharedPtr<Poco::JSON::Array>;
#else
// Explicitly instantiated shared pointer in JSON library
extern template class JSON_API Poco::SharedPtr<Poco::JSON::Array>;
#endif

namespace JSON {

class Object;


class JSON_API Array
	/// Represents a JSON array. Array provides a representation
	/// based on shared pointers and optimized for performance. It is possible to
	/// convert Array to Poco::Dynamic::Array. Conversion requires copying and therefore
	/// has performance penalty; the benefit is in improved syntax, eg:
	///
	///    // use pointers to avoid copying
	///    using namespace Poco::JSON;
	///    std::string json = "[ {\"test\" : 0}, { \"test1\" : [1, 2, 3], \"test2\" : 4 } ]";
	///    Parser parser;
	///    Var result = parser.parse(json);
	///    Array::Ptr arr = result.extract<Array::Ptr>();
	///    Object::Ptr object = arr->getObject(0); // object == {\"test\" : 0}
	///    int i = object->getElement<int>("test"); // i == 0;
	///    Object::Ptr subObject = arr->getObject(1); // subObject == {\"test\" : 0}
	///    Array subArr::Ptr = subObject->getArray("test1"); // subArr == [1, 2, 3]
	///    i = result = subArr->get(0); // i == 1;
	///
	///    // copy/convert to Poco::Dynamic::Array
	///    Poco::Dynamic::Array da = *arr;
	///    i = da[0]["test"];     // i == 0
	///    i = da[1]["test1"][1]; // i == 2
	///    i = da[1]["test2"];    // i == 4
	/// ----
{
public:
	using ValueVec = std::vector<Dynamic::Var>;
	using Iterator = std::vector<Dynamic::Var>::iterator;
	using ConstIterator = std::vector<Dynamic::Var>::const_iterator;
	using Ptr = SharedPtr<Array>;

	Array(int options = 0);
		/// Creates an empty Array.
		///
		/// If JSON_ESCAPE_UNICODE is specified, when the object is
		/// stringified, all unicode characters will be escaped in the
		/// resulting string.

	Array(const Array& copy);
		/// Creates an Array by copying another one.

	Array(Array&& other) noexcept;
		/// Move constructor

	Array& operator = (const Array& other);
		/// Assignment operator.

	Array& operator = (Array&& other) noexcept;
		/// Move assignment operator.

	~Array();
		/// Destroys the Array.

	void setEscapeUnicode(bool escape = true);
		/// Sets the flag for escaping unicode.

	bool getEscapeUnicode() const;
		/// Returns the flag for escaping unicode.
	
	void setLowercaseHex(bool lowercaseHex);
		/// Sets the flag for using lowercase hex numbers

	bool getLowercaseHex() const;
		/// Returns the flag for using lowercase hex numbers

	ValueVec::const_iterator begin() const;
		/// Returns the begin iterator for values.

	ValueVec::const_iterator end() const;
		/// Returns the end iterator for values.

	Dynamic::Var get(unsigned int index) const;
		/// Retrieves the element at the given index.
		/// Will return an empty value when the element doesn't exist.

	Array::Ptr getArray(unsigned int index) const;
		/// Retrieves an array. When the element is not
		/// an Array or doesn't exist, an empty SharedPtr is returned.

	template<typename T>
	T getElement(unsigned int index) const
		/// Retrieves an element and tries to convert it to the
		/// template type. The convert<T> method of
		/// Dynamic is called which can also throw
		/// exceptions for invalid values.
		/// Note: This will not work for an array or an object.
	{
		Dynamic::Var value = get(index);
		return value.convert<T>();
	}

	SharedPtr<Object> getObject(unsigned int index) const;
		/// Retrieves an object. When the element is not
		/// an object or doesn't exist, an empty SharedPtr is returned.

	std::size_t size() const;
		/// Returns the size of the array.

	bool empty() const;
 		/// Returns true if the array is empty, false otherwise.

	bool isArray(unsigned int index) const;
		/// Returns true when the element is an array.

	bool isArray(const Dynamic::Var& value) const;
		/// Returns true when the element is an array.

	bool isArray(ConstIterator& value) const;
		/// Returns true when the element is an array.

	bool isNull(unsigned int index) const;
		/// Returns true when the element is null or
		/// when the element doesn't exist.

	bool isObject(unsigned int index) const;
		/// Returns true when the element is an object.

	bool isObject(const Dynamic::Var& value) const;
		/// Returns true when the element is an object.

	bool isObject(ConstIterator& value) const;
		/// Returns true when the element is an object.

	template<typename T>
	T optElement(unsigned int index, const T& def) const
		/// Returns the element at the given index. When
		/// the element is null, doesn't exist or can't
		/// be converted to the given type, the default
		/// value will be returned
	{
		T value = def;
		if (index < _values.size())
		{
			try
			{
				value = _values[index].convert<T>();
			}
			catch (...)
			{
				// Default value is returned.
			}
		}
		return value;
	}

	Array& add(const Dynamic::Var& value);
		/// Add the given value to the array

	Array& set(unsigned int index, const Dynamic::Var& value);
		/// Update the element on the given index to specified value

	void stringify(std::ostream& out, unsigned int indent = 0, int step = -1) const;
		/// Prints the array to out. When indent has zero value,
		/// the array will be printed without newline breaks and spaces between elements.

	void remove(unsigned int index);
		/// Removes the element on the given index.

	operator const Poco::Dynamic::Array& () const;
		/// Conversion operator to Dynamic::Array.

	static Poco::Dynamic::Array makeArray(const JSON::Array::Ptr& arr);
		/// Utility function for creation of array.

	void clear();
		/// Clears the contents of the array.

private:
	void resetDynArray() const;

	using ArrayPtr = SharedPtr<Poco::Dynamic::Array>;

	ValueVec         _values;
	mutable ArrayPtr _pArray;
	mutable bool     _modified;
	// Note:
	//  The reason we have this flag here (rather than as argument to stringify())
	//  is because Array can be returned stringified from a Dynamic::Var:toString(),
	//  so it must know whether to escape unicode or not.
	bool             _escapeUnicode;
	bool             _lowercaseHex;
};


//
// inlines
//

inline void Array::setEscapeUnicode(bool escape)
{
	_escapeUnicode = escape;
}


inline bool Array::getEscapeUnicode() const
{
	return _escapeUnicode;
}

inline void Array::setLowercaseHex(bool lowercaseHex)
{
	_lowercaseHex = lowercaseHex;
}


inline bool Array::getLowercaseHex() const
{
	return _lowercaseHex;
}


inline Array::ValueVec::const_iterator Array::begin() const
{
	return _values.begin();
}


inline Array::ValueVec::const_iterator Array::end() const

{
	return _values.end();
}


inline std::size_t Array::size() const
{
	return static_cast<std::size_t>(_values.size());
}


inline bool Array::empty() const
{
	return _values.empty();
}


inline bool Array::isArray(unsigned int index) const
{
	Dynamic::Var value = get(index);
	return isArray(value);
}


inline bool Array::isArray(const Dynamic::Var& value) const
{
	return value.type() == typeid(Array::Ptr);
}


inline bool Array::isArray(ConstIterator& it) const
{
	return it!= end() && isArray(*it);
}


inline Array& Array::add(const Dynamic::Var& value)
{
	_values.push_back(value);
	_modified = true;
	return *this;
}


inline Array& Array::set(unsigned int index, const Dynamic::Var& value)
{
	if (index >= _values.size()) _values.resize(index + 1);
	_values[index] = value;
	_modified = true;
	return *this;
}


inline void Array::remove(unsigned int index)
{
	_values.erase(_values.begin() + index);
}


} } // namespace Poco::JSON


namespace Poco {
namespace Dynamic {


template <>
class VarHolderImpl<JSON::Array::Ptr>: public VarHolder
{
public:
	VarHolderImpl(const JSON::Array::Ptr& val): _val(val)
	{
	}

	~VarHolderImpl() override = default;

	const std::type_info& type() const override
	{
		return typeid(JSON::Array::Ptr);
	}

	void convert(Int8&) const override
	{
		throw BadCastException();
	}

	void convert(Int16&) const override
	{
		throw BadCastException();
	}

	void convert(Int32&) const override
	{
		throw BadCastException();
	}

	void convert(Int64&) const override
	{
		throw BadCastException();
	}

	void convert(UInt8&) const override
	{
		throw BadCastException();
	}

	void convert(UInt16&) const override
	{
		throw BadCastException();
	}

	void convert(UInt32&) const override
	{
		throw BadCastException();
	}

	void convert(UInt64&) const override
	{
		throw BadCastException();
	}

	void convert(bool& value) const override
	{
		value = !_val.isNull() && _val->size() > 0;
	}

	void convert(float&) const override
	{
		throw BadCastException();
	}

	void convert(double&) const override
	{
		throw BadCastException();
	}

	void convert(char&) const override
	{
		throw BadCastException();
	}

	void convert(std::string& s) const override
	{
		std::ostringstream oss;
		_val->stringify(oss);
		s = oss.str();
	}

	void convert(DateTime& /*val*/) const override
	{
		throw BadCastException("Cannot convert Array to DateTime");
	}

	void convert(LocalDateTime& /*ldt*/) const override
	{
		throw BadCastException("Cannot convert Array to LocalDateTime");
	}

	void convert(Timestamp& /*ts*/) const override
	{
		throw BadCastException("Cannot convert Array to Timestamp");
	}

	VarHolder* clone(Placeholder<VarHolder>* pVarHolder = nullptr) const override
	{
		return cloneHolder(pVarHolder, _val);
	}

	const JSON::Array::Ptr& value() const
	{
		return _val;
	}

	bool isInteger() const override
	{
		return false;
	}

	bool isSigned() const override
	{
		return false;
	}

	bool isNumeric() const override
	{
		return false;
	}

	bool isString() const override
	{
		return false;
	}

private:
	JSON::Array::Ptr _val;
};


template <>
class VarHolderImpl<JSON::Array>: public VarHolder
{
public:
	VarHolderImpl(const JSON::Array& val): _val(val)
	{
	}

	~VarHolderImpl() override = default;

	const std::type_info& type() const override
	{
		return typeid(JSON::Array);
	}

	void convert(Int8&) const override
	{
		throw BadCastException();
	}

	void convert(Int16&) const override
	{
		throw BadCastException();
	}

	void convert(Int32&) const override
	{
		throw BadCastException();
	}

	void convert(Int64&) const override
	{
		throw BadCastException();
	}

	void convert(UInt8&) const override
	{
		throw BadCastException();
	}

	void convert(UInt16&) const override
	{
		throw BadCastException();
	}

	void convert(UInt32&) const override
	{
		throw BadCastException();
	}

	void convert(UInt64&) const override
	{
		throw BadCastException();
	}

	void convert(bool& value) const override
	{
		value = _val.size() > 0;
	}

	void convert(float&) const override
	{
		throw BadCastException();
	}

	void convert(double&) const override
	{
		throw BadCastException();
	}

	void convert(char&) const override
	{
		throw BadCastException();
	}

	void convert(std::string& s) const override
	{
		std::ostringstream oss;
		_val.stringify(oss);
		s = oss.str();
	}

	void convert(DateTime& /*val*/) const override
	{
		throw BadCastException("Cannot convert Array to DateTime");
	}

	void convert(LocalDateTime& /*ldt*/) const override
	{
		throw BadCastException("Cannot convert Array to LocalDateTime");
	}

	void convert(Timestamp& /*ts*/) const override
	{
		throw BadCastException("Cannot convert Array to Timestamp");
	}

	VarHolder* clone(Placeholder<VarHolder>* pVarHolder = nullptr) const override
	{
		return cloneHolder(pVarHolder, _val);
	}

	const JSON::Array& value() const
	{
		return _val;
	}

	bool isInteger() const override
	{
		return false;
	}

	bool isSigned() const override
	{
		return false;
	}

	bool isNumeric() const override
	{
		return false;
	}

	bool isString() const override
	{
		return false;
	}

private:
	JSON::Array _val;
};


} } // namespace Poco::Dynamic


#endif // JSON_Array_INCLUDED
