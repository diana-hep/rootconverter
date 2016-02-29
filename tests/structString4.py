treeType = TreeType(Struct(TString))

fill = r"""
struct wrapper {
  TString x;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.x = TString("one");
t->Fill();
wrap_x.x = TString("two");
t->Fill();
wrap_x.x = TString("three");
t->Fill();
wrap_x.x = TString("four");
t->Fill();
wrap_x.x = TString("five");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "x", "type": "string"}]}
         }]}

json = [{"wrap_x": {"x": "one"}},
        {"wrap_x": {"x": "two"}},
        {"wrap_x": {"x": "three"}},
        {"wrap_x": {"x": "four"}},
        {"wrap_x": {"x": "five"}}]
