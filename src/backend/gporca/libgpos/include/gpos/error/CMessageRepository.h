//---------------------------------------------------------------------------
//	Cloudberry Database
//	Copyright (C) 2008 - 2010, Cloudberry, Inc.
//
//	@filename:
//		CMessageRepository.h
//
//	@doc:
//		Error message repository;
//---------------------------------------------------------------------------
#ifndef GPOS_CMessageRepository_H
#define GPOS_CMessageRepository_H

#include "gpos/error/CMessageTable.h"

namespace gpos
{
//---------------------------------------------------------------------------
//	@class:
//		CMessageRepository
//
//	@doc:
//		Stores and loads all error messages by locale
//
//---------------------------------------------------------------------------
class CMessageRepository
{
private:
	// global singleton
	static CMessageRepository *m_repository;

	// memory pool
	CMemoryPool *m_mp;

	// short hand for Table of Message Tables (TMT)
	using TMT = CSyncHashtable<CMessageTable, ELocale>;

	// short hand for TMT accessor
	using TMTAccessor = CSyncHashtableAccessByKey<CMessageTable, ELocale>;

	// basic hash table
	TMT m_hash_table;

	// init basic directory
	void InitDirectory(CMemoryPool *mp);

	// install message table for a given locale
	void AddMessageTable(ELocale locale);

	// pre-load standard messages
	void LoadStandardMessages();

public:
	// ctor
	CMessageRepository(CMemoryPool *mp);

	// dtor
	~CMessageRepository() = default;

	// lookup message by error/local
	CMessage *LookupMessage(CException exc, ELocale locale);

	// add individual message
	void AddMessage(ELocale locale, CMessage *msg);

	// initializer for global singleton
	static GPOS_RESULT Init();

	// accessor for global singleton
	static CMessageRepository *GetMessageRepository();

	void Shutdown();

};	// class CMessageRepository
}  // namespace gpos

#endif	// !GPOS_CMessageRepository_H

// EOF
