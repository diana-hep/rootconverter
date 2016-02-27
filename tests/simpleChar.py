treeType = TreeType(Char_t)

skip = "ROOT can't distinguish Char_t numbers from Char_t* strings?"

fill = r"""
TTree *t = new TTree("t", "");
char x;
t->Branch("x", &x, "x/C");
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
        {"x": -128},
        {"x": -1}]
