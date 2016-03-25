#ifndef STATICLIB_H
#define STATICLIB_H

#include <stdint.h>

#ifndef DATAWALKER_H
enum SchemaInstruction { dummy = 0 };
typedef void (*SchemaBuilder)(SchemaInstruction schemaInstruction, const void *data);
#endif // DATAWALKER_H

extern "C" {
  void resetSignals();
  void loadLibrary(const char *lib);

  void *newTreeWalker(const char *fileLocation, const char *treeLocation, const char *avroNamespace);
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
  const char *stringJSON(void *treeWalker);

  void buildSchema(void *treeWalker, SchemaBuilder schemaBuilder);
  int getDataSize(const void *fieldWalker, const void *address);
  const void *getData(const void *fieldWalker, const void *address, int index);
  void copyToBuffer(void *treeWalker, int64_t entry, int microBatchSize, void *buffer, long size);
}

#endif // STATICLIB_H
