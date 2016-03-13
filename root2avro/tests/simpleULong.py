treeType = TreeType(ULong_t)

fill = r"""
TTree *t = new TTree("t", "");
unsigned long x;
t->Branch("x", &x, "x/l");
x = 1;
t->Fill();
x = 2;
t->Fill();
x = 3;
t->Fill();
x = 4;
t->Fill();
x = 5;
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "double"}]}   # double, not long, because unsigned values exceed Avro's long specification

json = [{"x": 1},
        {"x": 2},
        {"x": 3},
        {"x": 4},
        {"x": 5}]
