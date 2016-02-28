treeType = TreeType(Vector(Vector(StdString)))

skip = "no dictionary available for this type"

fill = r"""
TTree *t = new TTree("t", "");
std::vector<std::vector<std::string> > x;
t->Branch("x", &x);

x = {};
t->Fill();

x = {{}, {"one"}};
t->Fill();

x = {{}, {"one"}, {"one", "two"}};
t->Fill();

x = {{"one", "two"}, {"one"}, {}};
t->Fill();

x = {{"one", "two", "three"}};
t->Fill();

x = {{"one"}, {"two"}, {"three"}};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": []},
        {"x": [[], ["one"]]},
        {"x": [[], ["one"], ["one", "two"]]},
        {"x": [["one", "two"], ["one"], []]},
        {"x": [["one", "two", "three"]]},
        {"x": [["one"], ["two"], ["three"]]}]
