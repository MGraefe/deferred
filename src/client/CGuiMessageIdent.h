// client/CGuiMessageIdent.h
// client/CGuiMessageIdent.h
//CGuiMessageIdent.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__client__CGuiMessageIdent_H__
#define deferred__client__CGuiMessageIdent_H__

#include <string>

class MessageIdent
{
public:
	MessageIdent(const std::string &callerName, int msgId) :
		m_callerName(callerName),
		m_msgId(msgId)
	{}

	inline int getMsgId() const {
		return m_msgId;
	}

	inline const std::string &getCallerName() const {
		return m_callerName;
	}

	//Comparison operator for std::map and the like, first compares message id, then string (should be faster)
	inline bool operator<(const MessageIdent &other) const {
		if(getMsgId() != other.getMsgId())
			return getMsgId() < other.getMsgId();
		else
			return getCallerName() < other.getCallerName();
	}

	inline bool operator==(const MessageIdent &other) const {
		return getMsgId() == other.getMsgId() && getCallerName() == other.getCallerName();
	}

	inline bool operator!=(const MessageIdent &other) const {
		return !(*this == other);
	}

private:
	const std::string m_callerName;
	const int m_msgId;
};

#endif // deferred__client__CGuiMessageIdent_H__