#ifndef STATICLIB_H
#define STATICLIB_H

extern "C" {
  void *newTreeWalker(const char *fileLocation, const char *treeLocation, const char *avroNamespace, int numLibs, const char **libs);
  void reset(void *treeWalker, const char *fileLocation);
  bool valid(void *treeWalker);
  const char *errorMessage(void *treeWalker);

  bool next(void *treeWalker);
  long numEntriesInCurrentTree(void *treeWalker);
  void setEntryInCurrentTree(void *treeWalker, long entry);

  bool resolved(void *treeWalker);
  void resolve(void *treeWalker);
  const char *repr(void *treeWalker);
  const char *avroSchema(void *treeWalker);
  void printJSON(void *treeWalker);
  bool printAvroHeaderOnce(void *treeWalker, const char *codec, int blockSize);
  bool printAvro(void *treeWalker);
  void closeAvro(void *treeWalker);
}

#endif // STATICLIB_H
