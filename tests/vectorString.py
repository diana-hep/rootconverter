treeType = TreeType(Vector(CharBrackets(10)))

skip = "this doesn't make retrievable vector of strings (try t->Scan() to see what I mean)"

fill = r"""
TTree *t = new TTree("t", "");
std::vector<const char*> x;
t->Branch("x", &x);

x = {"one", "two", "three"};
t->Fill();

x = {"uno", "dos", "tres"};
t->Fill();

x = {"un", "deux", "trois"};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": ["one", "two", "three"]},
        {"x": ["uno", "dos", "tres"]},
        {"x": ["un", "deux", "trois"]}]
