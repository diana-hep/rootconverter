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

treeType = TreeType(Int_t, Double_t, CharBrackets(10))

fill = r"""
TTree *t = new TTree("t", "");
int x;
double y;
char z[10];
t->Branch("x", &x, "x/I");
t->Branch("y", &y, "y/D");
t->Branch("z", &z, "z/C");
x = 1;
y = 1.1;
strcpy(z, "one");
t->Fill();
x = 2;
y = 2.2;
strcpy(z, "two");
t->Fill();
x = 3;
y = 3.3;
strcpy(z, "three");
t->Fill();
x = 4;
y = 4.4;
strcpy(z, "four");
t->Fill();
x = 5;
y = 5.5;
strcpy(z, "five");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "int"},
                     {"name": "y", "type": "double"},
                     {"name": "z", "type": "string"}]}

json = [{"x": 1, "y": 1.1, "z": "one"},
        {"x": 2, "y": 2.2, "z": "two"},
        {"x": 3, "y": 3.3, "z": "three"},
        {"x": 4, "y": 4.4, "z": "four"},
        {"x": 5, "y": 5.5, "z": "five"}]
