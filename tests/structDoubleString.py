treeType = TreeType(Struct(Double_t, CharBrackets(10)))

skip = "ROOT doens't allocate the array, and if ->Get() is called differently (->Get()->printJSON() instead of assigning an intermediate object), it only *partially* allocates the array"

fill = r"""
struct wrapper {
  double y;
  char x[10];
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.y = 1;
strcpy(wrap_x.x, "one");
t->Fill();
wrap_x.y = 2;
strcpy(wrap_x.x, "two");
t->Fill();
wrap_x.y = 3;
strcpy(wrap_x.x, "three");
t->Fill();
wrap_x.y = 4;
strcpy(wrap_x.x, "four");
t->Fill();
wrap_x.y = 5;
strcpy(wrap_x.x, "five");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "y", "type": "double"},
                        {"name": "x", "type": "string"}]}
         }]}

json = [{"wrap_x": {"y": 1, "x": "one"}},
        {"wrap_x": {"y": 2, "x": "two"}},
        {"wrap_x": {"y": 3, "x": "three"}},
        {"wrap_x": {"y": 4, "x": "four"}},
        {"wrap_x": {"y": 5, "x": "five"}}]
