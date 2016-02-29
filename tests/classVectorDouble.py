treeType = TreeType(Class(Vector(Double_t)))

fill = r"""
class wrapper {
public:
  std::vector<double> x;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.x = {};
t->Fill();
wrap_x.x = {1};
t->Fill();
wrap_x.x = {1, 2};
t->Fill();
wrap_x.x = {1, 2, 3};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "x", "type": {"type": "array", "items": "double"}}]}
         }]}

json = [{"wrap_x": {"x": []}},
        {"wrap_x": {"x": [1]}},
        {"wrap_x": {"x": [1, 2]}},
        {"wrap_x": {"x": [1, 2, 3]}}]
