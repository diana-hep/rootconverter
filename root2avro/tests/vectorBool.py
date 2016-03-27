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

treeType = TreeType(Vector(Bool_t))

fill = r"""
TTree *t = new TTree("t", "");
std::vector<bool> x;
t->Branch("x", &x);

x = {};
t->Fill();

x = {true};
t->Fill();

x = {true, false};
t->Fill();

x = {true, false, false};
t->Fill();

x = {false, false, false};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "boolean"}}]}

json = [{"x": []},
        {"x": [True]},
        {"x": [True, False]},
        {"x": [True, False, False]},
        {"x": [False, False, False]}]
