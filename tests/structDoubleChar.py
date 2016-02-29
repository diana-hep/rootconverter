treeType = TreeType(Struct(Double_t, Char_t))

fill = r"""
struct wrapper {
  double y;
  char x;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.y = 1;
wrap_x.x = 1;
t->Fill();
wrap_x.y = 2;
wrap_x.x = 2;
t->Fill();
wrap_x.y = 3;
wrap_x.x = 3;
t->Fill();
wrap_x.y = 4;
wrap_x.x = 4;
t->Fill();
wrap_x.y = 5;
wrap_x.x = 5;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "y", "type": "double"},
                        {"name": "x", "type": "int"}]}
         }]}

json = [{"wrap_x": {"y": 1, "x": 1}},
        {"wrap_x": {"y": 2, "x": 2}},
        {"wrap_x": {"y": 3, "x": 3}},
        {"wrap_x": {"y": 4, "x": 4}},
        {"wrap_x": {"y": 5, "x": 5}}]
