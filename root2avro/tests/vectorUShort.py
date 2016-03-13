treeType = TreeType(Vector(UShort_t))

fill = r"""
TTree *t = new TTree("t", "");
std::vector<unsigned short> x;
t->Branch("x", &x);
x = {};
t->Fill();
x = {10};
t->Fill();
x = {100, 101};
t->Fill();
x = {20, 21, 22};
t->Fill();
x = {200, 201, 202};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "int"}}]}

json = [{"x": []},
        {"x": [10]},
        {"x": [100, 101]},
        {"x": [20, 21, 22]},
        {"x": [200, 201, 202]}]
