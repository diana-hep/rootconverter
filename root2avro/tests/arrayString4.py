#!/usr/bin/env python

# Copyright 2016 Jim Pivarski
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

treeType = TreeType(Array(TString, 3))

skip = "ROOT doesn't recognize TString"

fill = r"""
TTree *t = new TTree("t", "");
TString x[3];
t->Branch("x", &x);

x[0] = TString("one");
x[1] = TString("two");
x[2] = TString("three");
t->Fill();

x[0] = TString("uno");
x[1] = TString("dos");
x[2] = TString("tres");
t->Fill();

x[0] = TString("un");
x[1] = TString("deux");
x[2] = TString("trois");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": ["one", "two", "three"]},
        {"x": ["uno", "dos", "tres"]},
        {"x": ["un", "deux", "trois"]}]
