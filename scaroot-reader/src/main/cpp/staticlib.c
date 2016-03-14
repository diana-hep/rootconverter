#include "datawalker.h"
#include "staticlib.h"

void *addVectorString(void *vectorString, const char *str) {
  std::vector<std::string> *l;  

  if (vectorString == nullptr)
    l = new std::vector<std::string>;
  else
    l = (std::vector<std::string>*)vectorString;

  l->push_back(std::string(str));

  return l;
}

void *newTreeWalker(const char *fileLocation, const char *treeLocation, const char *avroNamespace, void *libs) {
  std::vector<std::string> *l;
  if (libs == nullptr)
    l = new std::vector<std::string>;
  else
    l = (std::vector<std::string>*)libs;

  TreeWalker *out = new TreeWalker(std::string(fileLocation), std::string(treeLocation), std::string(avroNamespace), *l);
  delete l;
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

void buildSchema(void *treeWalker, SchemaBuilder schemaBuilder) {
  TreeWalker *tw = (TreeWalker*)treeWalker;
  tw->buildSchema(schemaBuilder);
}
