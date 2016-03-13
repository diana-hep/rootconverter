treeType = TreeType(Vector(TString))

fill = r"""
TTree *t = new TTree("t", "");
std::vector<TString> x;
t->Branch("x", &x);

x = {};
t->Fill();

x = {TString("one")};
t->Fill();

x = {TString("one"), TString("two")};
t->Fill();

x = {TString("one"), TString("two"), TString("three")};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": []},
        {"x": ["one"]},
        {"x": ["one", "two"]},
        {"x": ["one", "two", "three"]}]
