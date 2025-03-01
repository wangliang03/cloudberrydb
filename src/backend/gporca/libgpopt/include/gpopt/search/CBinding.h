//---------------------------------------------------------------------------
//	Cloudberry Database
//	Copyright (C) 2009 Cloudberry, Inc.
//
//	@filename:
//		CBinding.h
//
//	@doc:
//		Binding mechanism to extract expression from Memo according to pattern
//---------------------------------------------------------------------------
#ifndef GPOPT_CBinding_H
#define GPOPT_CBinding_H

#include "gpos/base.h"

#include "gpopt/operators/CExpression.h"

namespace gpopt
{
using namespace gpos;

// fwd declaration
class CGroupExpression;
class CGroup;

//---------------------------------------------------------------------------
//	@class:
//		CBinding
//
//	@doc:
//		Binding class used to iteratively generate expressions from the
//		memo so that they match a given pattern
//
//
//---------------------------------------------------------------------------
class CBinding
{
private:
	// initialize cursors of child expressions
	BOOL FInitChildCursors(CMemoryPool *mp, CGroupExpression *pgexpr,
						   CExpression *pexprPattern,
						   CExpressionArray *pdrgpexpr);

	// advance cursors of child expressions
	BOOL FAdvanceChildCursors(CMemoryPool *mp, CGroupExpression *pgexpr,
							  CExpression *pexprPattern, CExpression *pexprLast,
							  CExpressionArray *pdrgpexpr);

	// extraction of child expressions
	BOOL FExtractChildren(CMemoryPool *mp, CExpression *pexprPattern,
						  CGroupExpression *pgexprCursor,
						  CExpressionArray *pdrgpexpr);

	// move cursor
	static CGroupExpression *PgexprNext(CGroup *pgroup,
										CGroupExpression *pgexpr);

	// expand n-th child of pattern
	static CExpression *PexprExpandPattern(CExpression *pexpr, ULONG ulPos,
										   ULONG arity);

	// get binding for children
	BOOL FExtractChildren(CMemoryPool *mp, CGroupExpression *pgexpr,
						  CExpression *pexprPattern, CExpression *pexprLast,
						  CExpressionArray *pdrgpexprChildren);

	// extract binding from a group
	CExpression *PexprExtract(CMemoryPool *mp, CGroup *pgroup,
							  CExpression *pexprPattern,
							  CExpression *pexprLast);

	// build expression
	static CExpression *PexprFinalize(CMemoryPool *mp, CGroupExpression *pgexpr,
									  CExpressionArray *pdrgpexprChildren);

	// private copy ctor
	CBinding(const CBinding &);

public:
	// ctor
	CBinding() = default;

	// dtor
	~CBinding() = default;

	// extract binding from group expression
	CExpression *PexprExtract(CMemoryPool *mp, CGroupExpression *pgexpr,
							  CExpression *pexprPatetrn,
							  CExpression *pexprLast);

};	// class CBinding

}  // namespace gpopt

#endif	// !GPOPT_CBinding_H

// EOF
