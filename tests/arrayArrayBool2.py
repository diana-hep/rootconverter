treeType = TreeType(Array(Array(Bool_t, 2), 2))

fill = r"""
TTree *t = new TTree("t", "");

int d;
bool x[5][2];
x[0][0] = false;
x[0][1] = false;
x[1][0] = false;
x[1][1] = false;
x[2][0] = false;
x[2][1] = false;
x[3][0] = false;
x[3][1] = false;
x[4][0] = false;
x[4][1] = false;

t->Branch("d", &d, "d/I");
t->Branch("x", &x, "x[d][2]/O");

d = 0;
t->Fill();

d = 1;
x[0][0] = false;
x[0][1] = true;
t->Fill();

d = 2;
x[1][0] = true;
x[1][1] = true;
t->Fill();

d = 3;
x[2][0] = true;
x[2][1] = false;
t->Fill();

d = 4;
x[3][0] = false;
x[3][1] = false;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "boolean"}}}]}

json = [{"x": [[]]},
        {"x": [[False, True]]},
        {"x": [[False, True], [True, True]]},
        {"x": [[False, True], [True, True], [True, False]]},
        {"x": [[False, True], [True, True], [True, False], [False, False]]}]
