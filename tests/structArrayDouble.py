treeType = TreeType(Struct(Array(Double_t, 3)))

skip = "ROOT doens't allocate the array, and if ->Get() is called differently (->Get()->printJSON() instead of assigning an intermediate object), it only *partially* allocates the array"

fill = r"""
struct wrapper {
  double x[3];
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);

wrap_x.x[0] = 0;
wrap_x.x[1] = 1;
wrap_x.x[2] = 2;
t->Fill();

wrap_x.x[0] = 10;
wrap_x.x[1] = 11;
wrap_x.x[2] = 12;
t->Fill();

wrap_x.x[0] = 100;
wrap_x.x[1] = 101;
wrap_x.x[2] = 102;
t->Fill();

wrap_x.x[0] = -200;
wrap_x.x[1] = -201;
wrap_x.x[2] = -202;
t->Fill();

wrap_x.x[0] = -0;
wrap_x.x[1] = -1;
wrap_x.x[2] = -2;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "x", "type": {"type": "array", "items": "double"}}]}
         }]}

json = [{"wrap_x": {"x": [0, 1, 2]}},
        {"wrap_x": {"x": [10, 11, 12]}},
        {"wrap_x": {"x": [100, 101, 102]}},
        {"wrap_x": {"x": [-200, -201, -202]}},
        {"wrap_x": {"x": [0, -1, -2]}}]
