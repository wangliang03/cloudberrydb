//---------------------------------------------------------------------------
//	Cloudberry Database
//	Copyright (C) 2014 VMware, Inc. or its affiliates.
//
//	@filename:
//		CParseHandlerScalarArrayRefIndexList.h
//
//	@doc:
//		SAX parse handler class for parsing the list of arrayref indexes
//---------------------------------------------------------------------------

#ifndef GPDXL_CParseHandlerScalarScalarArrayRefIndexList_H
#define GPDXL_CParseHandlerScalarScalarArrayRefIndexList_H

#include "gpos/base.h"

#include "naucrates/dxl/parser/CParseHandlerScalarOp.h"

namespace gpdxl
{
using namespace gpos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerScalarArrayRefIndexList
//
//	@doc:
//		Parse handler class for parsing the list of arrayref indexes
//
//---------------------------------------------------------------------------
class CParseHandlerScalarArrayRefIndexList : public CParseHandlerScalarOp
{
private:
	// process the start of an element
	void StartElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname,		// element's qname
		const Attributes &attr					// element's attributes
		) override;

	// process the end of an element
	void EndElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname		// element's qname
		) override;

public:
	CParseHandlerScalarArrayRefIndexList(
		const CParseHandlerScalarArrayRefIndexList &) = delete;

	// ctor
	CParseHandlerScalarArrayRefIndexList(
		CMemoryPool *mp, CParseHandlerManager *parse_handler_mgr,
		CParseHandlerBase *parse_handler_root);
};
}  // namespace gpdxl

#endif	// !GPDXL_CParseHandlerScalarScalarArrayRefIndexList_H

// EOF
