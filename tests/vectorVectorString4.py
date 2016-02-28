treeType = TreeType(Vector(Vector(TString)))

skip = "no dictionary available for this type"

fill = r"""
TTree *t = new TTree("t", "");
std::vector<std::vector<TString> > x;
t->Branch("x", &x);

x = {};
t->Fill();

x = {{}, {TString("one")}};
t->Fill();

x = {{}, {TString("one")}, {TString("one"), TString("two")}};
t->Fill();

x = {{TString("one"), TString("two")}, {TString("one")}, {}};
t->Fill();

x = {{TString("one"), TString("two"), TString("three")}};
t->Fill();

x = {{TString("one")}, {TString("two")}, {TString("three")}};
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
