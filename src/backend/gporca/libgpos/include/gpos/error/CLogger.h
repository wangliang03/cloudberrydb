//---------------------------------------------------------------------------
//	Cloudberry Database
//	Copyright (C) 2010 Cloudberry, Inc.
//
//	@filename:
//		CLogger.h
//
//	@doc:
//		Partial implementation of interface class for logging
//---------------------------------------------------------------------------
#ifndef GPOS_CLogger_H
#define GPOS_CLogger_H

#include "gpos/error/ILogger.h"
#include "gpos/string/CWStringStatic.h"

namespace gpos
{
//---------------------------------------------------------------------------
//	@class:
//		CLogger
//
//	@doc:
//		Partial implementation of interface for abstracting logging primitives.
//
//---------------------------------------------------------------------------
class CLogger : public ILogger
{
	friend class ILogger;
	friend class CErrorHandlerStandard;

private:
	// buffer used to construct the log entry
	WCHAR m_entry[GPOS_LOG_ENTRY_BUFFER_SIZE];

	// buffer used to construct the log entry
	WCHAR m_msg[GPOS_LOG_MESSAGE_BUFFER_SIZE];

	// buffer used to retrieve system error messages
	CHAR m_retrieved_msg[GPOS_LOG_MESSAGE_BUFFER_SIZE];

	// entry buffer wrapper
	CWStringStatic m_entry_wrapper;

	// message buffer wrapper
	CWStringStatic m_msg_wrapper;

	// error logging information level
	ErrorInfoLevel m_info_level;

	// log message
	void Log(const WCHAR *msg, ULONG severity, const CHAR *filename,
			 ULONG line);

	// format log message
	void Format(const WCHAR *msg, ULONG severity, const CHAR *filename,
				ULONG line);

	// add date to message
	void AppendDate();

	// report logging failure
	void ReportFailure();

protected:
	// accessor for system error buffer
	CHAR *
	Msg()
	{
		return m_retrieved_msg;
	}

public:
	CLogger(const CLogger &) = delete;

	// ctor
	explicit CLogger(ErrorInfoLevel info_level = ILogger::EeilMsgHeaderStack);

	// dtor
	~CLogger() override;

	// error level accessor
	ErrorInfoLevel
	InfoLevel() const override
	{
		return m_info_level;
	}

	// set error info level
	void
	SetErrorInfoLevel(ErrorInfoLevel info_level) override
	{
		m_info_level = info_level;
	}

};	// class CLogger
}  // namespace gpos

#endif	// !GPOS_CLogger_H

// EOF
