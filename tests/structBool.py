treeType = TreeType(Struct(Bool_t))

skip = "a bug in bool handling prevents struct { bool x; } from having a TTClass::GetCollectionProxy"

fill = r"""
struct wrapper {
  bool x;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.x = true;
t->Fill();
wrap_x.x = false;
t->Fill();
wrap_x.x = true;
t->Fill();
wrap_x.x = true;
t->Fill();
wrap_x.x = false;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "x", "type": "boolean"}]}
         }]}

json = [{"wrap_x": {"x": True}},
        {"wrap_x": {"x": False}},
        {"wrap_x": {"x": True}},
        {"wrap_x": {"x": True}},
        {"wrap_x": {"x": False}}]
