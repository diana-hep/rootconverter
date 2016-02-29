treeType = TreeType(Struct(UChar_t, Double_t))

fill = r"""
struct wrapper {
  unsigned char x;
  double y;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.x = 1;
wrap_x.y = 1;
t->Fill();
wrap_x.x = 2;
wrap_x.y = 2;
t->Fill();
wrap_x.x = 3;
wrap_x.y = 3;
t->Fill();
wrap_x.x = 4;
wrap_x.y = 4;
t->Fill();
wrap_x.x = 5;
wrap_x.y = 5;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "x", "type": "int"},
                        {"name": "y", "type": "double"}]}
         }]}

json = [{"wrap_x": {"x": 1, "y": 1}},
        {"wrap_x": {"x": 2, "y": 2}},
        {"wrap_x": {"x": 3, "y": 3}},
        {"wrap_x": {"x": 4, "y": 4}},
        {"wrap_x": {"x": 5, "y": 5}}]
