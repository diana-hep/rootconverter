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

treeType = TreeType(Vector(UInt_t))

fill = r"""
TTree *t = new TTree("t", "");
std::vector<unsigned int> x;
t->Branch("x", &x);
x = {};
t->Fill();
x = {10};
t->Fill();
x = {100, 101};
t->Fill();
x = {20, 21, 22};
t->Fill();
x = {200, 201, 202};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "long"}}]}   # long, not int, because unsigned values exceed Avro's int specification

json = [{"x": []},
        {"x": [10]},
        {"x": [100, 101]},
        {"x": [20, 21, 22]},
        {"x": [200, 201, 202]}]
