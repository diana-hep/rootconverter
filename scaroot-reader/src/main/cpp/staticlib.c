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
#include "streamerToCode.h"

void *newTreeWalker(const char *fileLocation, const char *treeLocation, const char *avroNamespace) {
  TreeWalker *out = new TreeWalker(std::string(fileLocation), std::string(treeLocation), std::string(""), std::string(avroNamespace));
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

XRootD::XRootD(const char *urlstr) : url(urlstr) {
  fs = new TNetXNGSystem(url.c_str());
}

long XRootD::fileSize(const char *pathstr) {
  path = std::string(pathstr);
  if (fs->GetPathInfo(path.c_str(), buf) == 0)
    return buf.fSize;
  else
    return -1;
}

int XRootD::dirBegin(const char *pathstr) {
  path = std::string(pathstr);
  dir = fs->OpenDirectory(path.c_str());
}

const char *XRootD::dirEntry() {
  return fs->GetDirEntry(dir);
}

void XRootD::dirEnd() {
  fs->FreeDirectory(dir);
}

const char *XRootD::locate(const char *pathstr) {
  path = std::string(pathstr);
  if (fs->Locate(path.c_str(), endurl) == 0)
    return endurl.Data();
  else
    return empty.c_str();
}

void *xrootdFileSystem(const char *url) {
  XRootD *fs = new XRootD(url);
  return (void*)fs;
}

long xrootdFileSize(void *fs, const char *path) {
  XRootD *myfs = (XRootD*)fs;
  return myfs->fileSize(path);
}

void xrootdDirectoryBegin(void *fs, const char *path) {
  XRootD *myfs = (XRootD*)fs;
  myfs->dirBegin(path);
}

const char *xrootdDirectoryEntry(void *fs) {
  XRootD *myfs = (XRootD*)fs;
  return myfs->dirEntry();
}

void xrootdDirectoryEnd(void *fs) {
  XRootD *myfs = (XRootD*)fs;
  myfs->dirEnd();
}

const char *xrootdLocate(void *fs, const char *path) {
  XRootD *myfs = (XRootD*)fs;
  return myfs->locate(path);
}

const char *inferTypes(const char *fileLocation, const char *treeLocation) {
  std::vector<std::string> classNames;
  std::string errorMessage;
  std::string code = generateCodeFromStreamers(std::string(fileLocation), std::string(treeLocation), classNames, errorMessage);
  if (code.empty())
    return errorMessage.c_str();
  declareClasses(code, classNames);
  return "";
}
