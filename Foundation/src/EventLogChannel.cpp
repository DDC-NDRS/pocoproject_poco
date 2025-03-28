//
// EventLogChannel.cpp
//
// Library: Foundation
// Package: Logging
// Module:  EventLogChannel
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/EventLogChannel.h"
#include "Poco/Message.h"
#include "Poco/String.h"
#include "pocomsg.h"
#include "Poco/UnicodeConverter.h"


namespace Poco {


const std::string EventLogChannel::PROP_NAME    = "name";
const std::string EventLogChannel::PROP_HOST    = "host";
const std::string EventLogChannel::PROP_LOGHOST = "loghost";
const std::string EventLogChannel::PROP_LOGFILE = "logfile";


EventLogChannel::EventLogChannel():
	_logFile("Application"),
	_h(0)
{
	const DWORD maxPathLen = MAX_PATH + 1;
	wchar_t name[maxPathLen];
	int n = GetModuleFileNameW(NULL, name, maxPathLen);
	if (n > 0)
	{
		wchar_t* end = name + n - 1;
		while (end > name && *end != '\\') --end;
		if (*end == '\\') ++end;
		std::wstring uname(end);
		UnicodeConverter::toUTF8(uname, _name);
	}
}


EventLogChannel::EventLogChannel(const std::string& name):
	_name(name),
	_logFile("Application"),
	_h(0)
{
}


EventLogChannel::EventLogChannel(const std::string& name, const std::string& host):
	_name(name),
	_host(host),
	_logFile("Application"),
	_h(0)
{
}


EventLogChannel::~EventLogChannel()
{
	try
	{
		close();
	}
	catch (...)
	{
		poco_unexpected();
	}
}


void EventLogChannel::open()
{
	setUpRegistry();
	std::wstring uhost;
	UnicodeConverter::toUTF16(_host, uhost);
	std::wstring uname;
	UnicodeConverter::toUTF16(_name, uname);
	_h = RegisterEventSourceW(uhost.empty() ? NULL : uhost.c_str(), uname.c_str());
	if (!_h) throw SystemException("cannot register event source");
}


void EventLogChannel::close()
{
	if (_h) DeregisterEventSource(_h);
	_h = 0;
}


void EventLogChannel::log(const Message& msg)
{
	if (!_h) open();
	std::wstring utext;
	UnicodeConverter::toUTF16(msg.getText(), utext);
	const wchar_t* pMsg = utext.c_str();
	ReportEventW(_h, getType(msg), getCategory(msg), POCO_MSG_LOG, NULL, 1, 0, &pMsg, NULL);
}


void EventLogChannel::setProperty(const std::string& name, const std::string& value)
{
	if (icompare(name, PROP_NAME) == 0)
		_name = value;
	else if (icompare(name, PROP_HOST) == 0)
		_host = value;
	else if (icompare(name, PROP_LOGHOST) == 0)
		_host = value;
	else if (icompare(name, PROP_LOGFILE) == 0)
		_logFile = value;
	else
		Channel::setProperty(name, value);
}


std::string EventLogChannel::getProperty(const std::string& name) const
{
	if (icompare(name, PROP_NAME) == 0)
		return _name;
	else if (icompare(name, PROP_HOST) == 0)
		return _host;
	else if (icompare(name, PROP_LOGHOST) == 0)
		return _host;
	else if (icompare(name, PROP_LOGFILE) == 0)
		return _logFile;
	else
		return Channel::getProperty(name);
}


int EventLogChannel::getType(const Message& msg)
{
	switch (msg.getPriority())
	{
	case Message::PRIO_TRACE:
	case Message::PRIO_DEBUG:
	case Message::PRIO_INFORMATION:
		return EVENTLOG_INFORMATION_TYPE;
	case Message::PRIO_NOTICE:
	case Message::PRIO_WARNING:
		return EVENTLOG_WARNING_TYPE;
	default:
		return EVENTLOG_ERROR_TYPE;
	}
}


int EventLogChannel::getCategory(const Message& msg)
{
	switch (msg.getPriority())
	{
	case Message::PRIO_TRACE:
		return POCO_CTG_TRACE;
	case Message::PRIO_DEBUG:
		return POCO_CTG_DEBUG;
	case Message::PRIO_INFORMATION:
		return POCO_CTG_INFORMATION;
	case Message::PRIO_NOTICE:
		return POCO_CTG_NOTICE;
	case Message::PRIO_WARNING:
		return POCO_CTG_WARNING;
	case Message::PRIO_ERROR:
		return POCO_CTG_ERROR;
	case Message::PRIO_CRITICAL:
		return POCO_CTG_CRITICAL;
	case Message::PRIO_FATAL:
		return POCO_CTG_FATAL;
	default:
		return 0;
	}
}


void EventLogChannel::setUpRegistry() const
{
	std::string key = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\";
	key.append(_logFile);
	key.append("\\");
	key.append(_name);
	HKEY hKey;
	DWORD disp;
	std::wstring ukey;
	UnicodeConverter::toUTF16(key, ukey);
	DWORD rc = RegCreateKeyExW(HKEY_LOCAL_MACHINE, ukey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &disp);
	if (rc != ERROR_SUCCESS) return;

	if (disp == REG_CREATED_NEW_KEY)
	{
		std::wstring path;
		#if defined(POCO_DLL)
			#if defined(_DEBUG)
				#if defined(_WIN64) && !defined(POCO_CMAKE)
					path = findLibrary(L"PocoFoundation64d.dll");
				#else
					path = findLibrary(L"PocoFoundationd.dll");
				#endif
			#else
				#if defined(_WIN64) && !defined(POCO_CMAKE)
					path = findLibrary(L"PocoFoundation64.dll");
				#else
					path = findLibrary(L"PocoFoundation.dll");
				#endif
			#endif
		#endif

		if (path.empty())
			path = findLibrary(L"PocoMsg.dll");

		if (!path.empty())
		{
			DWORD count = 8;
			DWORD types = 7;
			RegSetValueExW(hKey, L"CategoryMessageFile", 0, REG_SZ, (const BYTE*) path.c_str(), static_cast<DWORD>(sizeof(wchar_t)*(path.size() + 1)));
			RegSetValueExW(hKey, L"EventMessageFile", 0, REG_SZ, (const BYTE*) path.c_str(), static_cast<DWORD>(sizeof(wchar_t)*(path.size() + 1)));
			RegSetValueExW(hKey, L"CategoryCount", 0, REG_DWORD, (const BYTE*) &count, static_cast<DWORD>(sizeof(count)));
			RegSetValueExW(hKey, L"TypesSupported", 0, REG_DWORD, (const BYTE*) &types, static_cast<DWORD>(sizeof(types)));
		}
	}
	RegCloseKey(hKey);
}


std::wstring EventLogChannel::findLibrary(const wchar_t* name)
{
	std::wstring path;
	HMODULE dll = LoadLibraryW(name);
	if (dll)
	{
		const DWORD maxPathLen = MAX_PATH + 1;
		wchar_t moduleName[maxPathLen];
		int n = GetModuleFileNameW(dll, moduleName, maxPathLen);
		if (n > 0) path = moduleName;
		FreeLibrary(dll);
	}
	return path;
}


} // namespace Poco
