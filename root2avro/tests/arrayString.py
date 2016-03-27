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

treeType = TreeType(Array(CharBrackets(10), 3))

skip = "this doesn't make retrievable arrays of strings (try t->Scan() to see what I mean)"

fill = r"""
TTree *t = new TTree("t", "");
const char* x[3];
t->Branch("x", &x, "x[3]/C");

std::string one("one");
std::string two("two");
std::string three("three");
x[0] = one.c_str();
x[1] = two.c_str();
x[2] = three.c_str();
t->Fill();

std::string uno("uno");
std::string dos("dos");
std::string tres("tres");
x[0] = uno.c_str();
x[1] = dos.c_str();
x[2] = tres.c_str();
t->Fill();

std::string un("un");
std::string deux("deux");
std::string trois("trois");
x[0] = un.c_str();
x[1] = deux.c_str();
x[2] = trois.c_str();
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": ["one", "two", "three"]},
        {"x": ["uno", "dos", "tres"]},
        {"x": ["un", "deux", "trois"]}]
