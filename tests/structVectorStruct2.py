treeType = TreeType(Struct(Double_t, Vector(Struct(Double_t))))

skip = "when executed in the ROOT interpreter, it works, but when run as a standalone executable, the \"outer\" class's constructor never gets called and the inner data are corrupted"

name = "this one is recursive (recursive types, non-recursive data)"

fill = r"""
struct recursive;

struct recursive {
  double label;
  std::vector<recursive> children;
};

TTree *t = new TTree("t", "");
recursive x;

t->Branch("x", &x);

x.label = 1;
t->Fill();

recursive y;
y.label = 2;
x.children.push_back(y);
t->Fill();

recursive z;
z.label = 3;
recursive y2;
y2.label = 2;
y2.children.push_back(z);
x.children.clear();
x.children.push_back(y2);
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type":
            {"type": "record",
             "name": "recursive",
             "fields": [{"name": "label", "type": "double"},
                        {"name": "children", "type": {"type": "array", "items": "recursive"}}]}
         }]}

json = [{"x": {"label": 1, "children": []}},
        {"x": {"label": 1, "children": [{"label": 2, "children": []}]}},
        {"x": {"label": 1, "children": [{"label": 2, "children": [{"label": 3, "children": []}]}]}}]
