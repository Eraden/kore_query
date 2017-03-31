/**
 * @file serialize.h
 * @author Adrian Eraden Woźniak
 * @date 21.12.2016
 * @brief Transform db exec JSON into response JSON
 * @example
 * @code
 *  JSONPath path[3] = {
 *      { .type=JSON_STRING, .name="accounts" },
 *      { .type=JSON_NUMBER, .index=0 },
 *      { .type=JSON_UNDEFINED, .name=NULL }
 *  };
 *  JSON *root = Database_exec(query);
 *  JSON *accounts = JSON_alloc(JSON_OBJECT);
 *  kore_serialization_scrapeAndMerge(accounts, root, path, JSON_DEEP);
 * @endcode
 */

#pragma once

#include "./json.h"

#ifdef KORE_SERIALIZE_DEBUG
#define KORE_SERIALIZE_INFO(format, info) kore_log(LOG_INFO, format, info);
#else
#define KORE_SERIALIZE_INFO(format, info)
#endif

/**
 * Lookup for path in source and then merge it to target.
 * @param target json object or array
 * @param source json object or array
 * @param path to source object or array
 */
int kore_serialization_scrapeAndMerge(JSON *target, JSON *source, JSONPath *path, JSONCloneType deep);

/**
 * Lookup for paths in target and source, then merge source to target.
 *
 * @param target json object or array
 * @param source json object or array
 * @param oldPath in source object or array
 * @param newPath in target object or array
 *
 * @note
 *  This will always use simple clone on children
 * @code
 *  // JSON *target == { "users": [] }
 *  // JSON *source == { "hello": { "world": [ "John" ] } }
 *  JSONPath oldPath[4] = {
 *      { .type=JSON_STRING, .name="hello" },
 *      { .type=JSON_STRING, .name="world" },
 *      { .type=JSON_UNDEFINED, .name=NULL }
 *  };
 *  JSONPath newPath[2] = {
 *      { .type=JSON_STRING, .name="users" },
 *      { .type=JSON_UNDEFINED, .name=NULL }
 *  };
 *  char succeed = kore_serialization_mergePaths(target, source, oldPath, newPath);
 *  // JSON *target == { "users": [] } because array will not be cloned deeply
 * @endcode
 */
int kore_serialization_mergePaths(JSON *target, JSON *source, JSONPath *oldPath, JSONPath *newPath);
