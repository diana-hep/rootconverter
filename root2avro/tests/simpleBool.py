treeType = TreeType(Bool_t)

fill = r"""
TTree *t = new TTree("t", "");
bool x;
t->Branch("x", &x, "x/O");
x = false;
t->Fill();
x = true;
t->Fill();
x = false;
t->Fill();
x = true;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "boolean"}]}

json = [{"x": False},
        {"x": True},
        {"x": False},
        {"x": True}]
