treeType = TreeType(Struct(Bool_t, Double_t))

fill = r"""
struct wrapper {
  bool x;
  double y;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.x = true;
wrap_x.y = 1;
t->Fill();
wrap_x.x = false;
wrap_x.y = 2;
t->Fill();
wrap_x.x = true;
wrap_x.y = 3;
t->Fill();
wrap_x.x = true;
wrap_x.y = 4;
t->Fill();
wrap_x.x = false;
wrap_x.y = 5;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "x", "type": "boolean"},
                        {"name": "y", "type": "double"}]}
         }]}

json = [{"wrap_x": {"x": True, "y": 1}},
        {"wrap_x": {"x": False, "y": 2}},
        {"wrap_x": {"x": True, "y": 3}},
        {"wrap_x": {"x": True, "y": 4}},
        {"wrap_x": {"x": False, "y": 5}}]
