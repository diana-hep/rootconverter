treeType = TreeType(Struct(Short_t))

fill = r"""
struct wrapper {
  short x;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.x = 1;
t->Fill();
wrap_x.x = 2;
t->Fill();
wrap_x.x = 3;
t->Fill();
wrap_x.x = 4;
t->Fill();
wrap_x.x = 5;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "x", "type": "int"}]}
         }]}

json = [{"wrap_x": {"x": 1}},
        {"wrap_x": {"x": 2}},
        {"wrap_x": {"x": 3}},
        {"wrap_x": {"x": 4}},
        {"wrap_x": {"x": 5}}]
