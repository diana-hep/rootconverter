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

treeType = TreeType(CharStar)

skip = "this doesn't make retrievable strings (try t->Scan() to see what I mean)"

fill = r"""
TTree *t = new TTree("t", "");
const char *x;
t->Branch("x", &x, "x/C");

std::string one("one");
x = one.c_str();
t->Fill();

std::string two("two");
x = two.c_str();
t->Fill();

std::string three("three");
x = three.c_str();
t->Fill();

std::string four("four");
x = four.c_str();
t->Fill();

std::string five("five");
x = five.c_str();
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "string"}]}

json = [{"x": "one"},
        {"x": "two"},
        {"x": "three"},
        {"x": "four"},
        {"x": "five"}]
