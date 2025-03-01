//---------------------------------------------------------------------------
//	Cloudberry Database
//	Copyright (C) 2009 Cloudberry, Inc.
//
//	@filename:
//		CBinding.cpp
//
//	@doc:
//		Implementation of Binding structure
//---------------------------------------------------------------------------

#include "gpopt/search/CBinding.h"

#include "gpos/base.h"

#include "gpopt/operators/CPattern.h"
#include "gpopt/search/CGroupProxy.h"
#include "gpopt/search/CMemo.h"

using namespace gpopt;

//---------------------------------------------------------------------------
//	@function:
//		CBinding::PgexprNext
//
//	@doc:
//		Move cursor within a group (initialize if NULL)
//
//---------------------------------------------------------------------------
CGroupExpression *
CBinding::PgexprNext(CGroup *pgroup, CGroupExpression *pgexpr)
{
	CGroupProxy gp(pgroup);

	if (pgroup->FScalar())
	{
		// initialize
		if (nullptr == pgexpr)
		{
			return gp.PgexprFirst();
		}

		return gp.PgexprNext(pgexpr);
	}

	// for non-scalar group, we only consider logical expressions in bindings
	return gp.PgexprNextLogical(pgexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::PexprExpandPattern
//
//	@doc:
//		Pattern operators which match more than one operator need to be
//		passed around;
//		Given the pattern determine if we need to re-use the pattern operators;
//
//---------------------------------------------------------------------------
CExpression *
CBinding::PexprExpandPattern(CExpression *pexprPattern, ULONG ulPos,
							 ULONG arity)
{
	GPOS_ASSERT_IMP(pexprPattern->Pop()->FPattern(),
					!CPattern::PopConvert(pexprPattern->Pop())->FLeaf());

	// re-use tree pattern
	if (COperator::EopPatternTree == pexprPattern->Pop()->Eopid() ||
		COperator::EopPatternMultiTree == pexprPattern->Pop()->Eopid())
	{
		return pexprPattern;
	}

	// re-use first child if it is a multi-leaf/tree
	if (0 < pexprPattern->Arity() &&
		CPattern::FMultiNode((*pexprPattern)[0]->Pop()))
	{
		GPOS_ASSERT(pexprPattern->Arity() <= 2);

		if (ulPos == arity - 1)
		{
			// special-case last child
			return (*pexprPattern)[pexprPattern->Arity() - 1];
		}

		// otherwise re-use multi-leaf/tree child
		return (*pexprPattern)[0];
	}
	GPOS_ASSERT(pexprPattern->Arity() > ulPos);

	return (*pexprPattern)[ulPos];
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::PexprFinalize
//
//	@doc:
//		Assemble expression; substitute operator with pattern as necessary
//
//---------------------------------------------------------------------------
CExpression *
CBinding::PexprFinalize(CMemoryPool *mp, CGroupExpression *pgexpr,
						CExpressionArray *pdrgpexpr)
{
	COperator *pop = pgexpr->Pop();

	pop->AddRef();
	CExpression *pexpr =
		GPOS_NEW(mp) CExpression(mp, pop, pgexpr, pdrgpexpr, nullptr /* prpp */,
								 nullptr /*input_stats*/);

	return pexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::PexprExtract
//
//	@doc:
//		Extract a binding according to a given pattern;
//		Keep root node fixed;
//
//---------------------------------------------------------------------------
CExpression *
CBinding::PexprExtract(CMemoryPool *mp, CGroupExpression *pgexpr,
					   CExpression *pexprPattern, CExpression *pexprLast)
{
	GPOS_CHECK_ABORT;

	if (!pexprPattern->FMatchPattern(pgexpr))
	{
		// shallow matching fails
		return nullptr;
	}

	// the previously extracted pattern must have the same root
	GPOS_ASSERT_IMP(nullptr != pexprLast, pexprLast->Pgexpr() == pgexpr);

	COperator *popPattern = pexprPattern->Pop();
	if (popPattern->FPattern() && CPattern::PopConvert(popPattern)->FLeaf())
	{
		// return immediately; no deep extraction for leaf patterns
		pgexpr->Pop()->AddRef();
		return GPOS_NEW(mp) CExpression(mp, pgexpr->Pop(), pgexpr);
	}

	// for a scalar operator, there is always only one group expression in it's
	// group. scalar operators are required to derive the scalar properties only
	// and no xforms are applied to them (i.e no PxfsCandidates in scalar op)
	// specifically which will generate equivalent scalar operators in the same group.
	// so, if a scalar op been extracted once, there is no need to explore
	// all the child bindings, as the scalar properites will remain the same.
	if (nullptr != pexprLast && pgexpr->Pgroup()->FScalar())
	{
		// there is only 1 group expression in the scalar group
		GPOS_ASSERT(1 == pgexpr->Pgroup()->UlGExprs());

		// the last operator and the current group expression will be same
		// for a scalar operator, as there is only one group expression in
		// the group
		GPOS_ASSERT(pexprLast->Pop()->Eopid() == pgexpr->Pop()->Eopid());
		return nullptr;
	}

	CExpressionArray *pdrgpexpr = nullptr;
	ULONG arity = pgexpr->Arity();
	if (0 == arity && nullptr != pexprLast)
	{
		// no more bindings
		return nullptr;
	}
	else
	{
		// attempt binding to children
		pdrgpexpr = GPOS_NEW(mp) CExpressionArray(mp);
		if (!FExtractChildren(mp, pgexpr, pexprPattern, pexprLast, pdrgpexpr))
		{
			pdrgpexpr->Release();
			return nullptr;
		}
	}

	CExpression *pexpr = PexprFinalize(mp, pgexpr, pdrgpexpr);
	GPOS_ASSERT(nullptr != pexpr);

	return pexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::FInitChildCursors
//
//	@doc:
//		Initialize cursors of child expressions
//
//---------------------------------------------------------------------------
BOOL
CBinding::FInitChildCursors(CMemoryPool *mp, CGroupExpression *pgexpr,
							CExpression *pexprPattern,
							CExpressionArray *pdrgpexpr)
{
	GPOS_ASSERT(nullptr != pexprPattern);
	GPOS_ASSERT(nullptr != pdrgpexpr);

	const ULONG arity = pgexpr->Arity();

	// grab first expression from each cursor
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CGroup *pgroup = (*pgexpr)[ul];
		CExpression *pexprPatternChild =
			PexprExpandPattern(pexprPattern, ul, arity);
		CExpression *pexprNewChild = PexprExtract(mp, pgroup, pexprPatternChild,
												  nullptr /*pexprLastChild*/);

		if (nullptr == pexprNewChild)
		{
			// failure means we have no more expressions
			return false;
		}

		pdrgpexpr->Append(pexprNewChild);
	}

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::FAdvanceChildCursors
//
//	@doc:
//		Advance cursors of child expressions and populate the given array
//		with the next child expressions
//
//---------------------------------------------------------------------------
BOOL
CBinding::FAdvanceChildCursors(CMemoryPool *mp, CGroupExpression *pgexpr,
							   CExpression *pexprPattern,
							   CExpression *pexprLast,
							   CExpressionArray *pdrgpexpr)
{
	GPOS_ASSERT(nullptr != pexprPattern);
	GPOS_ASSERT(nullptr != pdrgpexpr);

	const ULONG arity = pgexpr->Arity();
	if (nullptr == pexprLast)
	{
		// first call, initialize cursors
		return FInitChildCursors(mp, pgexpr, pexprPattern, pdrgpexpr);
	}

	// could we advance a child's cursor?
	BOOL fCursorAdvanced = false;

	// number of exhausted cursors
	ULONG ulExhaustedCursors = 0;

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CGroup *pgroup = (*pgexpr)[ul];
		CExpression *pexprPatternChild =
			PexprExpandPattern(pexprPattern, ul, arity);
		CExpression *pexprNewChild = nullptr;

		if (fCursorAdvanced)
		{
			// re-use last extracted child expression
			(*pexprLast)[ul]->AddRef();
			pexprNewChild = (*pexprLast)[ul];
		}
		else
		{
			CExpression *pexprLastChild = (*pexprLast)[ul];
			GPOS_ASSERT(pgroup == pexprLastChild->Pgexpr()->Pgroup());

			// advance current cursor
			pexprNewChild =
				PexprExtract(mp, pgroup, pexprPatternChild, pexprLastChild);

			if (nullptr == pexprNewChild)
			{
				// cursor is exhausted, we need to reset it
				pexprNewChild = PexprExtract(mp, pgroup, pexprPatternChild,
											 nullptr /*pexprLastChild*/);
				ulExhaustedCursors++;
			}
			else
			{
				// advancing current cursor has succeeded
				fCursorAdvanced = true;
			}
		}
		GPOS_ASSERT(nullptr != pexprNewChild);

		pdrgpexpr->Append(pexprNewChild);
	}

	GPOS_ASSERT(ulExhaustedCursors <= arity);


	return ulExhaustedCursors < arity;
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::FExtractChildren
//
//	@doc:
//		For a given root, extract children into a dynamic array;
//		Allocates the array for the children as needed;
//
//---------------------------------------------------------------------------
BOOL
CBinding::FExtractChildren(CMemoryPool *mp, CGroupExpression *pgexpr,
						   CExpression *pexprPattern, CExpression *pexprLast,
						   CExpressionArray *pdrgpexpr)
{
	GPOS_CHECK_STACK_SIZE;
	GPOS_CHECK_ABORT;

	GPOS_ASSERT(nullptr != pexprPattern);
	GPOS_ASSERT(nullptr != pdrgpexpr);
	GPOS_ASSERT_IMP(pexprPattern->Pop()->FPattern(),
					!CPattern::PopConvert(pexprPattern->Pop())->FLeaf());
	GPOS_ASSERT(pexprPattern->FMatchPattern(pgexpr));

	ULONG arity = pgexpr->Arity();
	if (arity < pexprPattern->Arity())
	{
		// does not have enough children
		return false;
	}

	if (0 == arity)
	{
		GPOS_ASSERT(0 == pexprPattern->Arity());
		return true;
	}


	return FAdvanceChildCursors(mp, pgexpr, pexprPattern, pexprLast, pdrgpexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CBinding::PexprExtract
//
//	@doc:
//		Extract a binding according to a given pattern;
//		If no appropriate child pattern can be matched advance the root node
//		until group is exhausted;
//
//---------------------------------------------------------------------------
CExpression *
CBinding::PexprExtract(CMemoryPool *mp, CGroup *pgroup,
					   CExpression *pexprPattern, CExpression *pexprLast)
{
	GPOS_CHECK_STACK_SIZE;
	GPOS_CHECK_ABORT;

	GPOS_ASSERT(nullptr != pgroup);
	GPOS_ASSERT(nullptr != pexprPattern);

	CGroupExpression *pgexpr = nullptr;
	if (nullptr != pexprLast)
	{
		pgexpr = pexprLast->Pgexpr();
	}
	else
	{
		// init cursor
		pgexpr = PgexprNext(pgroup, nullptr);
	}
	GPOS_ASSERT(nullptr != pgexpr);

	COperator *popPattern = pexprPattern->Pop();
	if (popPattern->FPattern() && CPattern::PopConvert(popPattern)->FLeaf())
	{
		// for leaf patterns, we do not iterate on group expressions
		if (nullptr != pexprLast)
		{
			// if a leaf was extracted before, then group is exhausted
			return nullptr;
		}

		return PexprExtract(mp, pgexpr, pexprPattern, pexprLast);
	}

	// start position for next binding
	CExpression *pexprStart = pexprLast;
	do
	{
		if (pexprPattern->FMatchPattern(pgexpr))
		{
			CExpression *pexprResult =
				PexprExtract(mp, pgexpr, pexprPattern, pexprStart);
			if (nullptr != pexprResult)
			{
				return pexprResult;
			}
		}

		// move cursor and reset start position
		pgexpr = PgexprNext(pgroup, pgexpr);
		pexprStart = nullptr;

		GPOS_CHECK_ABORT;
	} while (nullptr != pgexpr);

	// group exhausted
	return nullptr;
}


// EOF
