/*-------------------------------------------------------------------------
 *
 * relfilenodemap.h
 *	  relfilenode to oid mapping cache.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/relfilenodemap.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef RELFILENODEMAP_H
#define RELFILENODEMAP_H

extern Oid	RelidByRelfilenode(Oid reltablespace, RelFileNodeId relfilenode);

#endif							/* RELFILENODEMAP_H */
