//---------------------------------------------------------------------------
//	Cloudberry Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalHashJoin.h
//
//	@doc:
//		Base hash join operator
//---------------------------------------------------------------------------
#ifndef GPOPT_CPhysicalHashJoin_H
#define GPOPT_CPhysicalHashJoin_H

#include "gpos/base.h"

#include "gpopt/base/CUtils.h"
#include "gpopt/operators/CPhysicalJoin.h"

namespace gpopt
{
// fwd declarations
class CDistributionSpecHashed;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalHashJoin
//
//	@doc:
//		Base hash join operator
//
//---------------------------------------------------------------------------
class CPhysicalHashJoin : public CPhysicalJoin
{
private:
	// the array of expressions from the outer relation
	// that are extracted from the hashing condition
	CExpressionArray *m_pdrgpexprOuterKeys;

	// the array of expressions from the inner relation
	// that are extracted from the hashing condition
	CExpressionArray *m_pdrgpexprInnerKeys;

	// Hash op families of the operators used in the join conditions
	IMdIdArray *m_hash_opfamilies;

	// array redistribute request sent to the first hash join child
	CDistributionSpecArray *m_pdrgpdsRedistributeRequests;

	// compute a distribution matching the distribution delivered by given child
	CDistributionSpec *PdsMatch(CMemoryPool *mp, CDistributionSpec *pds,
								ULONG ulSourceChildIndex) const;

protected:
	// compute required hashed distribution from the n-th child
	CDistributionSpecHashed *PdshashedRequired(CMemoryPool *mp,
											   ULONG child_index,
											   ULONG ulReqIndex) const;
	// create (redistribute, redistribute) optimization request
	CDistributionSpec *PdsRequiredRedistribute(CMemoryPool *mp,
											   CExpressionHandle &exprhdl,
											   CDistributionSpec *pdsInput,
											   ULONG child_index,
											   CDrvdPropArray *pdrgpdpCtxt,
											   ULONG ulOptReq) const;

	// create the set of redistribute requests to send to first hash join child
	void CreateHashRedistributeRequests(CMemoryPool *mp);

private:
	// create (non-singleton, replicate) optimization request
	CDistributionSpec *PdsRequiredReplicate(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CDistributionSpec *pdsInput, ULONG child_index,
		CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq, CReqdPropPlan *prppInput);

	// create a child hashed distribution request based on input hashed distribution,
	// return NULL if no such request can be created
	static CDistributionSpecHashed *PdshashedPassThru(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CDistributionSpecHashed *pdshashedInput, ULONG child_index,
		CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq);

	// check whether a hash key is nullable
	BOOL FNullableHashKey(ULONG ulKey, CColRefSet *pcrsNotNullInner,
						  BOOL fInner) const;

protected:
	// helper for computing a hashed distribution matching the given distribution
	CDistributionSpecHashed *PdshashedMatching(
		CMemoryPool *mp, CDistributionSpecHashed *pdshashed,
		ULONG ulSourceChild) const;

	// create (singleton, singleton) optimization request
	CDistributionSpec *PdsRequiredSingleton(CMemoryPool *mp,
											CExpressionHandle &exprhdl,
											CDistributionSpec *pdsInput,
											ULONG child_index,
											CDrvdPropArray *pdrgpdpCtxt) const;

	// check whether the hash keys from one child are nullable
	BOOL FNullableHashKeys(CColRefSet *pcrsNotNullInner, BOOL fInner) const;

	ULONG
	NumDistrReq() const
	{
		return m_pdrgpdsRedistributeRequests->Size();
	}

	// create optimization requests
	virtual void CreateOptRequests(CMemoryPool *mp);

public:
	CPhysicalHashJoin(const CPhysicalHashJoin &) = delete;

	// ctor
	CPhysicalHashJoin(CMemoryPool *mp, CExpressionArray *pdrgpexprOuterKeys,
					  CExpressionArray *pdrgpexprInnerKeys,
					  IMdIdArray *hash_opfamilies,
					  CXform::EXformId origin_xform = CXform::ExfSentinel);

	// dtor
	~CPhysicalHashJoin() override;

	// inner keys
	const CExpressionArray *
	PdrgpexprInnerKeys() const
	{
		return m_pdrgpexprInnerKeys;
	}

	// outer keys
	const CExpressionArray *
	PdrgpexprOuterKeys() const
	{
		return m_pdrgpexprOuterKeys;
	}

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required sort order of the n-th child
	COrderSpec *PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
							COrderSpec *posInput, ULONG child_index,
							CDrvdPropArray *pdrgpdpCtxt,
							ULONG ulOptReq) const override;

	// compute required rewindability of the n-th child
	CRewindabilitySpec *PrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									CRewindabilitySpec *prsRequired,
									ULONG child_index,
									CDrvdPropArray *pdrgpdpCtxt,
									ULONG ulOptReq) const override;

	// compute required distribution of the n-th child
	CDistributionSpec *PdsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   CDistributionSpec *pdsRequired,
								   ULONG child_index,
								   CDrvdPropArray *pdrgpdpCtxt,
								   ULONG ulOptReq) const override;

	CEnfdDistribution *Ped(CMemoryPool *mp, CExpressionHandle &exprhdl,
						   CReqdPropPlan *prppInput, ULONG child_index,
						   CDrvdPropArray *pdrgpdpCtxt,
						   ULONG ulDistrReq) override;

	//-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------

	// derive sort order
	COrderSpec *
	PosDerive(CMemoryPool *mp,
			  CExpressionHandle &  // exprhdl
	) const override
	{
		// hash join is not order-preserving
		return GPOS_NEW(mp) COrderSpec(mp);
	}

	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return order property enforcing type for this operator
	CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const override;


	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// execution order of children
	EChildExecOrder
	Eceo() const override
	{
		// TODO - ; 01/06/2014
		// obtain this property by through MD abstraction layer, similar to scalar properties

		// hash join in GPDB executes its inner (right) child first,
		// the optimization order of hash join children follows the execution order
		return EceoRightToLeft;
	}

	// conversion function
	static CPhysicalHashJoin *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(CUtils::FHashJoin(pop));

		return dynamic_cast<CPhysicalHashJoin *>(pop);
	}

};	// class CPhysicalHashJoin

}  // namespace gpopt

#endif	// !GPOPT_CPhysicalHashJoin_H

// EOF
