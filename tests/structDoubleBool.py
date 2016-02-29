treeType = TreeType(Struct(Double_t, Bool_t))

fill = r"""
struct wrapper {
  double y;
  bool x;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.y = 1;
wrap_x.x = true;
t->Fill();
wrap_x.y = 2;
wrap_x.x = false;
t->Fill();
wrap_x.y = 3;
wrap_x.x = true;
t->Fill();
wrap_x.y = 4;
wrap_x.x = true;
t->Fill();
wrap_x.y = 5;
wrap_x.x = false;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "y", "type": "double"},
                        {"name": "x", "type": "boolean"}]}
         }]}

json = [{"wrap_x": {"y": 1, "x": True}},
        {"wrap_x": {"y": 2, "x": False}},
        {"wrap_x": {"y": 3, "x": True}},
        {"wrap_x": {"y": 4, "x": True}},
        {"wrap_x": {"y": 5, "x": False}}]
