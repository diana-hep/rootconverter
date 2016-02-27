treeType = TreeType(Char_t)

note = "uses /B instead of /C to indicate that this is a number, not the beginning of a C-style string"

fill = r"""
TTree *t = new TTree("t", "");
char x;
t->Branch("x", &x, "x/B");
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
        {"x": -128},    # note the wrap-around
        {"x": -1}]
