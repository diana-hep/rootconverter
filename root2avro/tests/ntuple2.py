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

treeType = TreeType(Float_t, Float_t)

note = "using TNtuple interface, rather than TTree"

fill = r"""
TNtuple *t = new TNtuple("t", "", "x:y");
t->Fill(1, 1.1);
t->Fill(2, 2.2);
t->Fill(3, 3.3);
t->Fill(4, 4.4);
t->Fill(5, 5.5);
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "float"},
                     {"name": "y", "type": "float"}]}

json = [{"x": 1, "y": 1.1},
        {"x": 2, "y": 2.2},
        {"x": 3, "y": 3.3},
        {"x": 4, "y": 4.4},
        {"x": 5, "y": 5.5}]
