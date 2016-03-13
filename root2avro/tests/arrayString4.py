treeType = TreeType(Array(TString, 3))

skip = "ROOT doesn't recognize TString"

fill = r"""
TTree *t = new TTree("t", "");
TString x[3];
t->Branch("x", &x);

x[0] = TString("one");
x[1] = TString("two");
x[2] = TString("three");
t->Fill();

x[0] = TString("uno");
x[1] = TString("dos");
x[2] = TString("tres");
t->Fill();

x[0] = TString("un");
x[1] = TString("deux");
x[2] = TString("trois");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": ["one", "two", "three"]},
        {"x": ["uno", "dos", "tres"]},
        {"x": ["un", "deux", "trois"]}]
