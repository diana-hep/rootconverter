treeType = TreeType(Vector(Bool_t))

fill = r"""
TTree *t = new TTree("t", "");
std::vector<bool> x;
t->Branch("x", &x);

x = {};
t->Fill();

x = {true};
t->Fill();

x = {true, false};
t->Fill();

x = {true, false, false};
t->Fill();

x = {false, false, false};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "boolean"}}]}

json = [{"x": []},
        {"x": [True]},
        {"x": [True, False]},
        {"x": [True, False, False]},
        {"x": [False, False, False]}]
