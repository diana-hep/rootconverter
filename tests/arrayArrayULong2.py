treeType = TreeType(Array(Array(ULong_t, 2), 2))

fill = r"""
TTree *t = new TTree("t", "");

int d;
unsigned long x[5][2];
x[0][0] = 0;
x[0][1] = 0;
x[1][0] = 0;
x[1][1] = 0;
x[2][0] = 0;
x[2][1] = 0;
x[3][0] = 0;
x[3][1] = 0;
x[4][0] = 0;
x[4][1] = 0;

t->Branch("d", &d, "d/I");
t->Branch("x", &x, "x[d][2]/l");

d = 0;
t->Fill();

d = 1;
x[0][0] = 1;
x[0][1] = 2;
t->Fill();

d = 2;
x[1][0] = 3;
x[1][1] = 4;
t->Fill();

d = 3;
x[2][0] = 5;
x[2][1] = 6;
t->Fill();

d = 4;
x[3][0] = 7;
x[3][1] = 8;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "double"}}}]}

json = [{"x": [[]]},
        {"x": [[1, 2]]},
        {"x": [[1, 2], [3, 4]]},
        {"x": [[1, 2], [3, 4], [5, 6]]},
        {"x": [[1, 2], [3, 4], [5, 6], [7, 8]]}]
