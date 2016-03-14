#ifndef STATICLIB_H
#define STATICLIB_H

#include <stdint.h>

#ifndef DATAWALKER_H
enum SchemaElement { dummy = 0 };
typedef void (*SchemaBuilder)(SchemaElement schemaElement, const char *word);
#endif // DATAWALKER_H

extern "C" {
  void *addVectorString(void *vectorString, const char *str);

  void *newTreeWalker(const char *fileLocation, const char *treeLocation, const char *avroNamespace, void *libs);
  void reset(void *treeWalker, const char *fileLocation);
  bool valid(void *treeWalker);
  const char *errorMessage(void *treeWalker);

  bool next(void *treeWalker);
  int64_t numEntriesInCurrentTree(void *treeWalker);
  void setEntryInCurrentTree(void *treeWalker, int64_t entry);

  bool resolved(void *treeWalker);
  void resolve(void *treeWalker);
  const char *repr(void *treeWalker);
  const char *avroSchema(void *treeWalker);
  void printJSON(void *treeWalker);

  void buildSchema(void *treeWalker, SchemaBuilder schemaBuilder);
  // void buildData(void *treeWalker, DataBuilder dataBuilder);
}

#endif // STATICLIB_H
