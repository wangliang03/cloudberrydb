/*-------------------------------------------------------------------------
 *
 * print.h
 *	  definitions for nodes/print.c
 *
 *
 * Portions Copyright (c) 2007-2008, Cloudberry inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/nodes/print.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PRINT_H
#define PRINT_H

#include "nodes/plannodes.h"
#include "executor/tuptable.h"

#define nodeDisplay(x)		pprint(x)

extern char *plannode_type(Plan *p);
extern void print(const void *obj);
extern void pprint(const void *obj);
extern void elog_node_display(int lev, const char *title,
							  const void *obj, bool pretty);
extern char *format_node_dump(const char *dump);
extern char *pretty_format_node_dump(const char *dump);
extern void print_rt(const List *rtable);
extern void print_expr(const Node *expr, const List *rtable);
extern void print_pathkeys(const List *pathkeys, const List *rtable);
extern void print_tl(const List *tlist, const List *rtable);
extern void print_slot(TupleTableSlot *slot);
extern void print_plan(Plan *p, Query *parsetree);

#endif							/* PRINT_H */
