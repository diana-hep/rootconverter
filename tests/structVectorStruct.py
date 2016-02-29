treeType = TreeType(Struct(Double_t, Vector(Struct(Double_t))))

skip = "when executed in the ROOT interpreter, it works, but when run as a standalone executable, the \"outer\" class's constructor never gets called and the inner data are corrupted"

fill = r"""
struct inner {
  double label;
};

struct outer {
  std::vector<inner> children;
};

TTree *t = new TTree("t", "");
outer x;
inner y, z;
y.label = 1;
z.label = 2;

t->Branch("x", &x);

t->Fill();

x.children.push_back(y);
t->Fill();

x.children.push_back(z);
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type":
            {"type": "record",
             "name": "outer",
             "fields": [{"name": "children", "type":
               {"type": "array", "items":
                  {"type": "record",
                   "name": "inner",
                   "fields": [{"name": "label", "type": "double"}]
                  }
               }}]
            }}]
         }

json = [{"x": {"children": []}},
        {"x": {"children": [{"label": 1}]}},
        {"x": {"children": [{"label": 1}, {"label": 2}]}}]
