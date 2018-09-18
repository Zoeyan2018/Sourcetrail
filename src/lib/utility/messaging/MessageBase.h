#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H

#include <ostream>
#include <sstream>

#include "types.h"
#include "utilityString.h"

class MessageBase
{
public:
	MessageBase()
		: m_id(s_nextId++)
		, m_isParallel(false)
		, m_isReplayed(false)
		, m_sendAsTask(true)
		, m_keepContent(false)
		, m_isLast(true)
		, m_isLogged(true)
	{
	}

	virtual ~MessageBase()
	{
	}

	virtual std::string getType() const = 0;
	virtual void dispatch() = 0;

	Id getId() const
	{
		return m_id;
	}

	bool sendAsTask() const
	{
		return m_sendAsTask;
	}

	void setSendAsTask(bool sendAsTask)
	{
		m_sendAsTask = sendAsTask;
	}

	bool isParallel() const
	{
		return m_isParallel;
	}

	void setIsParallel(bool isParallel)
	{
		m_isParallel = isParallel;
	}

	bool isReplayed() const
	{
		return m_isReplayed;
	}

	void setIsReplayed(bool isReplayed)
	{
		m_isReplayed = isReplayed;
	}

	bool isLast() const
	{
		return m_isLast;
	}

	void setIsLast(bool isLast)
	{
		m_isLast = isLast;
	}

	bool isLogged() const
	{
		return m_isLogged;
	}

	void setIsLogged(bool isLogged)
	{
		m_isLogged = isLogged;
	}

	void setKeepContent(bool keepContent)
	{
		m_keepContent = keepContent;
	}

	bool keepContent() const
	{
		return m_keepContent;
	}

	virtual void print(std::wostream& os) const = 0;

	std::wstring str() const
	{
		std::wstringstream ss;
		ss << utility::decodeFromUtf8(getType()) << L" ";
		print(ss);
		return ss.str();
	}

private:
	static Id s_nextId;

	Id m_id;

	bool m_isParallel;
	bool m_isReplayed;

	bool m_sendAsTask;
	bool m_keepContent;

	bool m_isLast;
	bool m_isLogged;
};

#endif // MESSAGE_BASE_H
