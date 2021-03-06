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

treeType = TreeType(Array(Array(UChar_t, 2), 2))

note = "uses [5][2]/b instead of /C to indicate that these are numbers, not a C-style string"

fill = r"""
TTree *t = new TTree("t", "");

int d;
unsigned char x[5][2];
x[0][0] = 0;
x[0][1] = 0;
x[1][0] = 0;
x[1][1] = 0;
x[2][0] = 0;
x[2][1] = 0;
x[3][0] = 0;
x[3][1] = 0;
x[4][0] = 0;
x[4][1] = 0;

t->Branch("d", &d, "d/I");
t->Branch("x", &x, "x[d][2]/b");

d = 0;
t->Fill();

d = 1;
x[0][0] = 1;
x[0][1] = 2;
t->Fill();

d = 2;
x[1][0] = 3;
x[1][1] = 4;
t->Fill();

d = 3;
x[2][0] = 5;
x[2][1] = 6;
t->Fill();

d = 4;
x[3][0] = 7;
x[3][1] = 8;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "d", "type": "int"},
                     {"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "int"}}}]}

json = [{"d": 0, "x": []},
        {"d": 1, "x": [[1, 2]]},
        {"d": 2, "x": [[1, 2], [3, 4]]},
        {"d": 3, "x": [[1, 2], [3, 4], [5, 6]]},
        {"d": 4, "x": [[1, 2], [3, 4], [5, 6], [7, 8]]}]
