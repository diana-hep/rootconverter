treeType = TreeType(Long_t)

fill = r"""
TTree *t = new TTree("t", "");
long x;
t->Branch("x", &x, "x/L");
x = 1;
t->Fill();
x = 2;
t->Fill();
x = 3;
t->Fill();
x = 4;
t->Fill();
x = 5;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "long"}]}

json = [{"x": 1},
        {"x": 2},
        {"x": 3},
        {"x": 4},
        {"x": 5}]
