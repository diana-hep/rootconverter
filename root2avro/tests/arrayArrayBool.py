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

treeType = TreeType(Array(Array(Bool_t, 2), 2))

fill = r"""
TTree *t = new TTree("t", "");
bool x[2][2];
t->Branch("x", &x, "x[2][2]/O");
x[0][0] = true;
x[0][1] = true;
x[1][0] = true;
x[1][1] = true;
t->Fill();

x[0][0] = true;
x[0][1] = true;
x[1][0] = true;
x[1][1] = false;
t->Fill();

x[0][0] = true;
x[0][1] = false;
x[1][0] = false;
x[1][1] = true;
t->Fill();

x[0][0] = false;
x[0][1] = false;
x[1][0] = false;
x[1][1] = false;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "boolean"}}}]}

json = [{"x": [[True, True], [True, True]]},
        {"x": [[True, True], [True, False]]},
        {"x": [[True, False], [False, True]]},
        {"x": [[False, False], [False, False]]}]
