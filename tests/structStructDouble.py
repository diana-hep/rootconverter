treeType = TreeType(Struct(Struct(Double_t)))

fill = r"""
struct inner {
  double x;
};

struct outer {
  inner y;
};

TTree *t = new TTree("t", "");
outer z;

t->Branch("z", &z);
z.y.x = 1;
t->Fill();
z.y.x = 2;
t->Fill();
z.y.x = 3;
t->Fill();
z.y.x = 4;
t->Fill();
z.y.x = 5;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "z", "type":
            {"type": "record",
             "name": "outer",
             "fields": [{"name": "y", "type":
               {"type": "record",
                "name": "inner",
                "fields": [{"name": "x", "type": "double"}]
               }}]
            }}]
         }

json = [{"z": {"y": {"x": 1}}},
        {"z": {"y": {"x": 2}}},
        {"z": {"y": {"x": 3}}},
        {"z": {"y": {"x": 4}}},
        {"z": {"y": {"x": 5}}}]
