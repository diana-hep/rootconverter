// Copyright 2016 Jim Pivarski
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "datawalker.h"
#include "staticlib.h"

#include "TNetXNGSystem.h"

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

void *xrootdFileSystem(const char *url) {
  return (void*)(new TNetXNGSystem(url));
}

long xrootdFileSize(void *fs, const char *path) {
  FileStat_t buf;
  ((TNetXNGSystem*)fs)->GetPathInfo(path, buf);
  return buf.fSize;
}

void *xrootdDirectoryIter(void *fs, const char *path) {
  return ((TNetXNGSystem*)fs)->OpenDirectory(path);
}

const char *xrootdDirectoryEntry(void *fs, void *dir) {
  return ((TNetXNGSystem*)fs)->GetDirEntry(dir);
}

void xrootdDirectoryFree(void *fs, void *dir) {
  return ((TNetXNGSystem*)fs)->FreeDirectory(dir);
}
