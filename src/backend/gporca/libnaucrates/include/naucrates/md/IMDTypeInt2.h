//---------------------------------------------------------------------------
//	Cloudberry Database
//	Copyright (C) 2014 VMware, Inc. or its affiliates.
//
//	@filename:
//		IMDTypeInt2.h
//
//	@doc:
//		Interface for INT2 types in the metadata cache
//---------------------------------------------------------------------------

#ifndef GPMD_IMDTypeInt2_H
#define GPMD_IMDTypeInt2_H

#include "gpos/base.h"

#include "naucrates/md/IMDType.h"

// fwd decl
namespace gpnaucrates
{
class IDatumInt2;
}

namespace gpmd
{
using namespace gpos;
using namespace gpnaucrates;


//---------------------------------------------------------------------------
//	@class:
//		IMDTypeInt2
//
//	@doc:
//		Interface for INT2 types in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTypeInt2 : public IMDType
{
public:
	// type id
	static ETypeInfo
	GetTypeInfo()
	{
		return EtiInt2;
	}

	ETypeInfo
	GetDatumType() const override
	{
		return IMDTypeInt2::GetTypeInfo();
	}

	// factory function for INT2 datums
	virtual IDatumInt2 *CreateInt2Datum(CMemoryPool *mp, SINT value,
										BOOL is_null) const = 0;
};

}  // namespace gpmd

#endif	// !GPMD_IMDTypeInt2_H

// EOF
