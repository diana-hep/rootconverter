#include "staticlib.h"
#include "datawalker.h"

void *newTreeWalker(const char *fileLocation, const char *treeLocation, const char *avroNamespace, int numLibs, const char **libs) {
  std::vector<std::string> vectorLibs;
  for (int i = 0;  i < numLibs;  i++)
    vectorLibs.push_back(std::string(libs[i]));
  TreeWalker *out = new TreeWalker(std::string(fileLocation), std::string(treeLocation), std::string(avroNamespace), vectorLibs);
  return out;
}

void reset(void *treeWalker, const char *fileLocation) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->reset(std::string(fileLocation));
}

bool valid(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->valid;
}

const char *errorMessage(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->errorMessage.c_str();
}

bool next(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->next();
}

long numEntriesInCurrentTree(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->numEntriesInCurrentTree();
}

void setEntryInCurrentTree(void *treeWalker, long entry) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->setEntryInCurrentTree(entry);
}

bool resolved(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->resolved();
}

void resolve(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->resolve();
}

const char *repr(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->repr().c_str();
}

const char *avroSchema(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->avroSchema().c_str();
}

void printJSON(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->printJSON();
}

bool printAvroHeaderOnce(void *treeWalker, const char *codec, int blockSize) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  std::string c = codec;
  return tw->printAvroHeaderOnce(c, blockSize);
}

bool printAvro(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->printAvro();
}

void closeAvro(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->closeAvro();
}
