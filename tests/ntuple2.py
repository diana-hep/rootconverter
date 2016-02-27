treeType = TreeType(Float_t, Float_t)

note = "using TNtuple interface, rather than TTree"

fill = r"""
TNtuple *t = new TNtuple("t", "", "x:y");
t->Fill(1, 1.1);
t->Fill(2, 2.2);
t->Fill(3, 3.3);
t->Fill(4, 4.4);
t->Fill(5, 5.5);
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "float"},
                     {"name": "y", "type": "float"}]}

json = [{"x": 1, "y": 1.1},
        {"x": 2, "y": 2.2},
        {"x": 3, "y": 3.3},
        {"x": 4, "y": 4.4},
        {"x": 5, "y": 5.5}]
