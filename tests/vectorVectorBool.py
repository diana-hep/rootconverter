treeType = TreeType(Vector(Vector(Bool_t)))

skip = "no dictionary available for this type"

fill = r"""
TTree *t = new TTree("t", "");
std::vector<std::vector<bool> > x;
t->Branch("x", &x);
x = {};
t->Fill();
x = {{}, {true}};
t->Fill();
x = {{}, {true}, {true, false}};
t->Fill();
x = {{1, false}, {true}, {}};
t->Fill();
x = {{true, false, true}};
t->Fill();
x = {{true}, {false}, {true}};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "boolean"}}}]}

json = [{"x": []},
        {"x": [[], [1]]},
        {"x": [[], [1], [1, 2]]},
        {"x": [[1, 2], [1], []]},
        {"x": [[1, 2, 3]]},
        {"x": [[1], [2], [3]]}]
