treeType = TreeType(Vector(Vector(UChar_t)))

skip = "no dictionary available for this type"

fill = r"""
TTree *t = new TTree("t", "");
std::vector<std::vector<unsigned char> > x;
t->Branch("x", &x);
x = {};
t->Fill();
x = {{}, {1}};
t->Fill();
x = {{}, {1}, {1, 2}};
t->Fill();
x = {{1, 2}, {1}, {}};
t->Fill();
x = {{1, 2, 3}};
t->Fill();
x = {{1}, {2}, {3}};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "int"}}}]}

json = [{"x": []},
        {"x": [[], [1]]},
        {"x": [[], [1], [1, 2]]},
        {"x": [[1, 2], [1], []]},
        {"x": [[1, 2, 3]]},
        {"x": [[1], [2], [3]]}]
