treeType = TreeType(TString)

fill = r"""
TTree *t = new TTree("t", "");
TString x;
t->Branch("x", &x);

x = TString("one");
t->Fill();

x = TString("two");
t->Fill();

x = TString("three");
t->Fill();

x = TString("four");
t->Fill();

x = TString("five");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "string"}]}

json = [{"x": "one"},
        {"x": "two"},
        {"x": "three"},
        {"x": "four"},
        {"x": "five"}]
