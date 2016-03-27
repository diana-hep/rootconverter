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

treeType = TreeType(UChar_t)

fill = r"""
TTree *t = new TTree("t", "");
unsigned char x;
t->Branch("x", &x, "x/b");
x = 0;
t->Fill();
x = 1;
t->Fill();
x = 2;
t->Fill();
x = 127;
t->Fill();
x = 128;
t->Fill();
x = 255;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "int"}]}

json = [{"x": 0},
        {"x": 1},
        {"x": 2},
        {"x": 127},
        {"x": 128},    # note the lack of wrap-around
        {"x": 255}]
