#ifndef STATICLIB_H
#define STATICLIB_H

#include <stdint.h>

typedef void (*callme)(void *data);

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
  bool printAvroHeaderOnce(void *treeWalker, const char *codec, int blockSize);
  bool printAvro(void *treeWalker);
  void closeAvro(void *treeWalker);

  void run(void *treeWalker, callme callback);
}

#endif // STATICLIB_H
