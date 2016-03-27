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

treeType = TreeType(Array(Array(Float_t, 2), 2))

fill = r"""
TTree *t = new TTree("t", "");
float x[2][2];
t->Branch("x", &x, "x[2][2]/F");
x[0][0] = 1;
x[0][1] = 2;
x[1][0] = 3;
x[1][1] = 4;
t->Fill();

x[0][0] = 11;
x[0][1] = 12;
x[1][0] = 13;
x[1][1] = 14;
t->Fill();

x[0][0] = 101;
x[0][1] = 102;
x[1][0] = 103;
x[1][1] = 104;
t->Fill();

x[0][0] = 21;
x[0][1] = 22;
x[1][0] = 23;
x[1][1] = 24;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "float"}}}]}

json = [{"x": [[1, 2], [3, 4]]},
        {"x": [[11, 12], [13, 14]]},
        {"x": [[101, 102], [103, 104]]},
        {"x": [[21, 22], [23, 24]]}]
