treeType = TreeType(Array(Array(UChar_t, 2), 2))

note = "uses [2][2]/b instead of /C to indicate that these are numbers, not a C-style string"

fill = r"""
TTree *t = new TTree("t", "");
unsigned char x[2][2];
t->Branch("x", &x, "x[2][2]/b");
x[0][0] = 1;
x[0][1] = 2;
x[1][0] = 3;
x[1][1] = 4;
t->Fill();

x[0][0] = 11;
x[0][1] = 12;
x[1][0] = 13;
x[1][1] = 14;
t->Fill();

x[0][0] = 101;
x[0][1] = 102;
x[1][0] = 103;
x[1][1] = 104;
t->Fill();

x[0][0] = 21;
x[0][1] = 22;
x[1][0] = 23;
x[1][1] = 24;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "int"}}}]}

json = [{"x": [[1, 2], [3, 4]]},
        {"x": [[11, 12], [13, 14]]},
        {"x": [[101, 102], [103, 104]]},
        {"x": [[21, 22], [23, 24]]}]