//
// MessageHeader.h
//
// Library: MongoDB
// Package: MongoDB
// Module:  MessageHeader
//
// Definition of the MessageHeader class.
//
// Copyright (c) 2012, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef MongoDB_MessageHeader_INCLUDED
#define MongoDB_MessageHeader_INCLUDED


#include "Poco/MongoDB/MongoDB.h"
#include "Poco/MongoDB/MessageHeader.h"


namespace Poco {
namespace MongoDB {


class Message; // Required to disambiguate friend declaration in MessageHeader.


class MongoDB_API MessageHeader
	/// Represents the message header which is always prepended to a
	/// MongoDB request or response message.
{
public:
	static constexpr Int32 MSG_HEADER_SIZE = 16;

	enum OpCode
	{
#if false
		// Opcodes deprecated in MongoDB 5.0
		OP_REPLY [[deprecated]] = 1,
		OP_UPDATE [[deprecated]] = 2001,
		OP_INSERT [[deprecated]] = 2002,
		OP_QUERY [[deprecated]] = 2004,
		OP_GET_MORE [[deprecated]] = 2005,
		OP_DELETE [[deprecated]] = 2006,
		OP_KILL_CURSORS [[deprecated]] = 2007,
#else
		OP_REPLY = 1,
		OP_UPDATE = 2001,
		OP_INSERT = 2002,
		OP_QUERY = 2004,
		OP_GET_MORE = 2005,
		OP_DELETE = 2006,
		OP_KILL_CURSORS = 2007,
#endif

		/// Opcodes supported in MongoDB 5.1 and later
		OP_COMPRESSED = 2012,
		OP_MSG = 2013
	};

	explicit MessageHeader(OpCode);
		/// Creates the MessageHeader using the given OpCode.

	virtual ~MessageHeader();
		/// Destroys the MessageHeader.

	void read(BinaryReader& reader);
		/// Reads the header using the given BinaryReader.

	void write(BinaryWriter& writer);
		/// Writes the header using the given BinaryWriter.

	Int32 getMessageLength() const;
		/// Returns the message length.

	OpCode opCode() const;
		/// Returns the OpCode.

	Int32 getRequestID() const;
		/// Returns the request ID of the current message.

	void setRequestID(Int32 id);
		/// Sets the request ID of the current message.

	Int32 responseTo() const;
		/// Returns the request id from the original request.

private:
	void setMessageLength(Int32 length);
		/// Sets the message length.

	Int32 _messageLength;
	Int32 _requestID;
	Int32 _responseTo;
	OpCode _opCode;

	friend class Message;
};


//
// inlines
//
inline MessageHeader::OpCode MessageHeader::opCode() const
{
	return _opCode;
}


inline Int32 MessageHeader::getMessageLength() const
{
	return _messageLength;
}


inline void MessageHeader::setMessageLength(Int32 length)
{
	poco_assert (_messageLength >= 0);
	_messageLength = MSG_HEADER_SIZE + length;
}


inline void MessageHeader::setRequestID(Int32 id)
{
	_requestID = id;
}


inline Int32 MessageHeader::getRequestID() const
{
	return _requestID;
}

inline Int32 MessageHeader::responseTo() const
{
	return _responseTo;
}


} } // namespace Poco::MongoDB


#endif // MongoDB_MessageHeader_INCLUDED
