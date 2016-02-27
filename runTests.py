#!/usr/bin/env python

import glob
import json
import os
import subprocess
import sys
from collections import OrderedDict as odict

if len(sys.argv) == 1:
    testFileNames = glob.glob("tests/*.py")
else:
    testFileNames = sys.argv[1:]

class TreeType:
    order = ["Primitive", "CharBrackets", "Array", "Vector", "Struct"]
    @staticmethod
    def index(instance):
        return TreeType.order.index(instance.__class__.__name__)
    def __init__(self, *tpes):
        self.tpes = sorted(tpes)
    def __repr__(self):
        return "TreeType(" + ", ".join(map(repr, self.tpes)) + ")"
    def __eq__(self, other):
        return isinstance(other, TreeType) and self.tpes == other.tpes
    def __cmp__(self, other):
        if self == other:
            return 0
        else:
            return -1 if self.tpes < other.tpes else 1
            
class Primitive:
    order = []
    def __init__(self, name):
        self.name = name
        self.order.append(name)
    def __repr__(self):
        return self.name
    def __eq__(self, other):
        return isinstance(other, Primitive) and self.name == other.name
    def __cmp__(self, other):
        if self == other:
            return 0
        elif not isinstance(other, Primitive):
            return -1 if TreeType.index(self) < TreeType.index(other) else 1
        else:
            return -1 if self.order.index(self.name) < self.order.index(other.name) else 1

Bool_t = Primitive("Bool_t")
Char_t = Primitive("Char_t")
Short_t = Primitive("Short_t")
Int_t = Primitive("Int_t")
Long_t = Primitive("Long_t")
Long64_t = Primitive("Long64_t")
UChar_t = Primitive("UChar_t")
UShort_t = Primitive("UShort_t")
UInt_t = Primitive("UInt_t")
ULong_t = Primitive("ULong_t")
ULong64_t = Primitive("ULong64_t")
Float_t = Primitive("Float_t")
Float16_t = Primitive("Float16_t")
Double_t = Primitive("Double_t")
Double32_t = Primitive("Double32_t")

TString = Primitive("TString")
CharStar = Primitive("CharStar")
class CharBrackets:
    def __init__(self, size):
        self.size = size
    def __repr__(self):
        return "CharBrackets(" + repr(self.size) + ")"
    def __eq__(self, other):
        return isinstance(other, CharBrackets) and self.size == other.size
    def __cmp__(self, other):
        if self == other:
            return 0
        elif not isinstance(other, CharBrackets):
            return -1 if TreeType.index(self) < TreeType.index(other) else 1
        else:
            return -1 if self.size < other.size else 1

class Array:
    def __init__(self, tpe, size):
        self.tpe = tpe
        self.size = size
    def __repr__(self):
        return "Array(" + repr(self.tpe) + ", " + repr(self.size) + ")"
    def __eq__(self, other):
        return isinstance(other, Array) and self.tpe == other.tpe and self.size == other.size
    def __cmp__(self, other):
        if self == other:
            return 0
        elif not isinstance(other, Array):
            return -1 if TreeType.index(self) < TreeType.index(other) else 1
        elif self.tpe == other.tpe:
            return -1 if self.size < other.size else 1
        else:
            return -1 if self.tpe < other.tpe else 1
            
class Vector:
    def __init__(self, tpe):
        self.tpe = tpe
    def __repr__(self):
        return "Vector(" + repr(self.tpe) + ")"
    def __eq__(self, other):
        return isinstance(other, Vector) and self.tpe == other.tpe
    def __cmp__(self, other):
        if self == other:
            return 0
        elif not isinstance(other, Vector):
            return -1 if TreeType.index(self) < TreeType.index(other) else 1
        else:
            return cmp(self.tpe, other.tpe)

class Struct:
    def __init__(self, *tpes):
        self.tpes = sorted(tpes)
    def __repr__(self):
        return "Struct(" + ", ".join(map(repr, self.tpes)) + ")"
    def __eq__(self, other):
        return isinstance(other, Struct) and self.tpes == other.tpes
    def __cmp__(self, other):
        if self == other:
            return 0
        elif not isinstance(other, Struct):
            return -1 if TreeType.index(self) < TreeType.index(other) else 1
        else:
            return cmp(self.tpes, other.tpes)

tests = []
for testFileName in testFileNames:
    testEnv = dict(vars(), testFileName=testFileName)
    exec open(testFileName).read() in testEnv
    if "treeType" not in testEnv or "fill" not in testEnv or "schema" not in testEnv or "json" not in testEnv:
        raise KeyError(testFileName + " should define treeType, fill, schema, and json")
    tests.append(testEnv)

tests.sort(key=lambda _: _["treeType"])

def same(one, two, eps):
    if isinstance(one, dict) and isinstance(two, dict) and set(one.keys()) == set(two.keys()):
        return all(same(one[key], two[key], eps) for key in one)
    elif isinstance(one, list) and isinstance(two, list) and len(one) == len(two):
        return all(same(x, y, eps) for x, y in zip(one, two))
    elif isinstance(one, basestring) and isinstance(two, basestring):
        return one == two
    elif isinstance(one, (int, long, float)) and isinstance(two, (int, long, float)):
        return abs(one - two) <= eps
    elif one is True and two is True:
        return True
    elif one is False and two is False:
        return True
    elif one is None and two is None:
        return True
    else:
        return False

def dumpsPretty(x):
    return json.dumps(x, sort_keys=True, indent=4, separators=(", ", ": "))

for test in tests:
    print repr(test["treeType"]), "in", test["testFileName"] + "...",
    sys.stdout.flush()
    if "notes" in test:
        print test["notes"],
    try:
        rootFile = os.path.join("build", os.path.split(test["testFileName"])[1].rsplit(".", 1)[0] + ".root")

        # build the root file if it doesn't exist or if it's older than the corresponding test
        if not os.path.exists(rootFile) or os.path.getmtime(rootFile) < os.path.getmtime(test["testFileName"]):
            command = ["root", "-l"]
            root = subprocess.Popen(command, stdin=subprocess.PIPE)
            root.stdin.write("TFile *tfile = new TFile(\"%s\", \"RECREATE\");\n" % rootFile)
            root.stdin.write(test["fill"] + "\n")
            root.stdin.write("tfile->Write();\n")
            root.stdin.write("tfile->Close();\n")
            root.stdin.write(".q\n")
            if root.wait() != 0:
                raise RuntimeError("root TTree filling failed with exit code %d" % root.returncode)

        command = ["build/root2avro", "--mode=schema", "file://" + rootFile, "t"]
        root2avro = subprocess.Popen(command, stdout=subprocess.PIPE)
        if root2avro.wait() != 0:
            raise RuntimeError("root2avro failed with exit code %d" % root.returncode)
        schemaResult = root2avro.stdout.read()

        try:
            schemaResultJson = json.loads(schemaResult)
        except ValueError as err:
            raise RuntimeError("root2avro --mode=schema produced bad JSON:\n\n%s" % schemaResult)

        if not same(schemaResultJson, test["schema"], 0):
            raise RuntimeError("root2avro produced the wrong JSON:\n\n%s\n\nExpected:\n\n%s" % (schemaResult, dumpsPretty(test["schema"])))

        command = ["build/root2avro", "--mode=json", "file://" + rootFile, "t"]
        root2avro = subprocess.Popen(command, stdout=subprocess.PIPE)
        if root2avro.wait() != 0:
            raise RuntimeError("root2avro --mode=json failed with exit code %d" % root.returncode)
        dataResult = root2avro.stdout.readlines()

        try:
            dataResultJson = map(json.loads, dataResult)
        except ValueError as err:
            raise RuntimeError("root2avro produced bad JSON:\n\n%s" % dataResult)

        if not same(dataResultJson, test["json"], 1e-5):
            raise RuntimeError("root2avro produced the wrong JSON:\n\n%s\n\nExpected:\n\n%s" % (dumpsPretty(dataResultJson), dumpsPretty(test["json"])))

    except Exception as err:
        print "FAILURE"
        print >> sys.stderr
        print >> sys.stderr, "File:      ", test["testFileName"]
        print >> sys.stderr, "Command:   ", " ".join(command)
        print >> sys.stderr
        raise
    else:
        print "SUCCESS"
