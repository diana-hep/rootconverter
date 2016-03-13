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
