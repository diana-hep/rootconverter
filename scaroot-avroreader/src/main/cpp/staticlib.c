#include "datawalker.h"
#include "staticlib.h"

void *newTreeWalker(const char *fileLocation, const char *treeLocation, const char *avroNamespace) {
  TreeWalker *out = new TreeWalker(std::string(fileLocation), std::string(treeLocation), std::string(avroNamespace));
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

int64_t numEntriesInCurrentTree(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  return tw->numEntriesInCurrentTree();
}

void setEntryInCurrentTree(void *treeWalker, int64_t entry) {
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
  tw->stringHolder = tw->repr();
  return tw->stringHolder.c_str();
}

const char *avroSchema(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->stringHolder = tw->avroSchema();
  return tw->stringHolder.c_str();
}

void printJSON(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->printJSON();
}

const char *stringJSON(void *treeWalker) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->stringHolder = tw->stringJSON();
  return tw->stringHolder.c_str();
}

void buildSchema(void *treeWalker, SchemaBuilder schemaBuilder) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->buildSchema(schemaBuilder);
}

int getDataSize(const void *dataProvider, const void *address) {
  DataProvider *dp = (DataProvider*)dataProvider;
  return dp->getDataSize(address);
}

const void *getData(const void *dataProvider, const void *address, int index) {
  DataProvider *dp = (DataProvider*)dataProvider;
  return dp->getData(address, index);
}

void copyToBuffer(void *treeWalker, int64_t entry, int microBatchSize, void *buffer, long size) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->copyToBuffer(entry, microBatchSize, buffer, (size_t)size);
}
