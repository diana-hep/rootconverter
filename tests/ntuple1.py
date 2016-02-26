treeType = TreeType(Float_t)

fill = r"""
TNtuple *t = new TNtuple("t", "", "x");
t->Fill(1);
t->Fill(2);
t->Fill(3);
t->Fill(4);
t->Fill(5);
"""

schema = {"type": "record",
          "name": "TTree",
          "fields": [{"name": "x", "type": "float"}]}

json = [{"x": 1},
        {"x": 2},
        {"x": 3},
        {"x": 4},
        {"x": 5}]
