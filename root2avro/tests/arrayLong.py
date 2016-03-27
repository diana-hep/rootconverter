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

treeType = TreeType(Array(Long_t, 3))

fill = r"""
TTree *t = new TTree("t", "");
long x[3];
t->Branch("x", &x, "x[3]/L");
x[0] = 0;
x[1] = 1;
x[2] = 2;
t->Fill();
x[0] = -10;
x[1] = 11;
x[2] = 12;
t->Fill();
x[0] = -100;
x[1] = -101;
x[2] = 102;
t->Fill();
x[0] = 20;
x[1] = -21;
x[2] = -22;
t->Fill();
x[0] = 200;
x[1] = -201;
x[2] = 202;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "long"}}]}

json = [{"x": [0, 1, 2]},
        {"x": [-10, 11, 12]},
        {"x": [-100, -101, 102]},
        {"x": [20, -21, -22]},
        {"x": [200, -201, 202]}]
