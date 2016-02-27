treeType = TreeType(Vector(CharBrackets(10)))

skip = "this doesn't make retrievable vector of strings (try t->Scan() to see what I mean)"

fill = r"""
TTree *t = new TTree("t", "");
std::vector<const char*> x;
t->Branch("x", &x);

x = {};
t->Fill();

x = {"one"};
t->Fill();

x = {"one", "two"};
t->Fill();

x = {"one", "two", "three"};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": []},
        {"x": ["one"]},
        {"x": ["one", "two"]},
        {"x": ["one", "two", "three"]}]
