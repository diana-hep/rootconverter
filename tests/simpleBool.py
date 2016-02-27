treeType = TreeType(Bool_t)

fill = r"""
TTree *t = new TTree("t", "");
int x;
t->Branch("x", &x, "x/O");
x = 0;
t->Fill();
x = 1;
t->Fill();
x = 0;
t->Fill();
x = 1;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "boolean"}]}

json = [{"x": False},
        {"x": True},
        {"x": False},
        {"x": True}]
