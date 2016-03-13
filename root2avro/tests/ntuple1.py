treeType = TreeType(Float_t)

note = "using TNtuple interface, rather than TTree"

fill = r"""
TNtuple *t = new TNtuple("t", "", "x");
t->Fill(1);
t->Fill(2);
t->Fill(3);
t->Fill(4);
t->Fill(5);
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "float"}]}

json = [{"x": 1},
        {"x": 2},
        {"x": 3},
        {"x": 4},
        {"x": 5}]
