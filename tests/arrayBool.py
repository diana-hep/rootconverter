treeType = TreeType(Array(Bool_t, 3))

fill = r"""
TTree *t = new TTree("t", "");
bool x[3];
t->Branch("x", &x, "x[3]/O");

x[0] = true;
x[1] = true;
x[2] = true;
t->Fill();

x[0] = true;
x[1] = true;
x[2] = false;
t->Fill();

x[0] = true;
x[1] = false;
x[2] = true;
t->Fill();

x[0] = true;
x[1] = false;
x[2] = false;
t->Fill();

x[0] = false;
x[1] = false;
x[2] = false;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "boolean"}}]}

json = [{"x": [True, True, True]},
        {"x": [True, True, False]},
        {"x": [True, False, True]},
        {"x": [True, False, False]},
        {"x": [False, False, False]}]
