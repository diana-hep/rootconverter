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

treeType = TreeType(Array(StdString, 3))

skip = "ROOT doesn't recognize std::string"

fill = r"""
TTree *t = new TTree("t", "");
std::string x[3];
t->Branch("x", &x);

x[0] = std::string("one");
x[1] = std::string("two");
x[2] = std::string("three");
t->Fill();

x[0] = std::string("uno");
x[1] = std::string("dos");
x[2] = std::string("tres");
t->Fill();

x[0] = std::string("un");
x[1] = std::string("deux");
x[2] = std::string("trois");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": ["one", "two", "three"]},
        {"x": ["uno", "dos", "tres"]},
        {"x": ["un", "deux", "trois"]}]
