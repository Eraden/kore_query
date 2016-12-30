#include "./serialize.h"

int
kore_serialization_scrapeAndMerge(
    JSON *target,
    JSON *source,
    JSONPath *path,
    JSONCloneType deep
) {
  if (!target) return 0;
  if (!source) return 0;
  JSON *currentSource = JSON_find(source, path);
  if (!currentSource) return 0;
  return JSON_mergeJSON(target, currentSource, deep);
}

int
kore_serialization_mergePaths(
    JSON *target,
    JSON *source,
    JSONPath *oldPath,
    JSONPath *newPath
) {
  JSON *currentTarget = JSON_find(target, newPath);
  JSON *currentSource = JSON_find(source, oldPath);
  if (!currentSource || !currentTarget) return 0;
  return JSON_mergeJSON(currentTarget, currentSource, JSON_SIMPLE);
}
