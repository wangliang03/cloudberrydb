//---------------------------------------------------------------------------
//	Cloudberry Database
//	Copyright (C) 2009 Cloudberry, Inc.
//
//	@filename:
//		CXformContext.h
//
//	@doc:
//		Context container passed to every application of a transformation
//---------------------------------------------------------------------------
#ifndef GPOPT_CXformContext_H
#define GPOPT_CXformContext_H

#include "gpos/base.h"
#include "gpos/common/CRefCount.h"

#include "gpopt/operators/CPatternTree.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CXformContext
//
//	@doc:
//		context container
//
//---------------------------------------------------------------------------
class CXformContext : public CRefCount
{
private:
	// Memory pool
	CMemoryPool *m_mp;

public:
	CXformContext(const CXformContext &) = delete;

	// ctor
	explicit CXformContext(CMemoryPool *mp) : m_mp(mp)
	{
	}

	// dtor
	~CXformContext() override = default;


	// accessor
	inline CMemoryPool *
	Pmp() const
	{
		return m_mp;
	}

};	// class CXformContext

}  // namespace gpopt


#endif	// !GPOPT_CXformContext_H

// EOF
