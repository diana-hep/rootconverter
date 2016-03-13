treeType = TreeType(Array(Short_t, 3))

fill = r"""
TTree *t = new TTree("t", "");
short x[3];
t->Branch("x", &x, "x[3]/S");
x[0] = 0;
x[1] = 1;
x[2] = 2;
t->Fill();
x[0] = -10;
x[1] = 11;
x[2] = 12;
t->Fill();
x[0] = -100;
x[1] = -101;
x[2] = 102;
t->Fill();
x[0] = 20;
x[1] = -21;
x[2] = -22;
t->Fill();
x[0] = 200;
x[1] = -201;
x[2] = 202;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "int"}}]}

json = [{"x": [0, 1, 2]},
        {"x": [-10, 11, 12]},
        {"x": [-100, -101, 102]},
        {"x": [20, -21, -22]},
        {"x": [200, -201, 202]}]
