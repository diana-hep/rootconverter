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

int d;
bool x[5][2];
x[0][0] = false;
x[0][1] = false;
x[1][0] = false;
x[1][1] = false;
x[2][0] = false;
x[2][1] = false;
x[3][0] = false;
x[3][1] = false;
x[4][0] = false;
x[4][1] = false;

t->Branch("d", &d, "d/I");
t->Branch("x", &x, "x[d][2]/O");

d = 0;
t->Fill();

d = 1;
x[0][0] = false;
x[0][1] = true;
t->Fill();

d = 2;
x[1][0] = true;
x[1][1] = true;
t->Fill();

d = 3;
x[2][0] = true;
x[2][1] = false;
t->Fill();

d = 4;
x[3][0] = false;
x[3][1] = false;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "d", "type": "int"},
                     {"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "boolean"}}}]}

json = [{"d": 0, "x": []},
        {"d": 1, "x": [[False, True]]},
        {"d": 2, "x": [[False, True], [True, True]]},
        {"d": 3, "x": [[False, True], [True, True], [True, False]]},
        {"d": 4, "x": [[False, True], [True, True], [True, False], [False, False]]}]
