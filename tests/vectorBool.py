treeType = TreeType(Vector(Bool_t))

fill = r"""
TTree *t = new TTree("t", "");
std::vector<bool> x;
t->Branch("x", &x);

x = {true, true, true};
t->Fill();

x = {true, true, false};
t->Fill();

x = {true, false, true};
t->Fill();

x = {true, false, false};
t->Fill();

x = {false, false, false};
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
