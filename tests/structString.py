treeType = TreeType(Struct(CharBrackets(10)))

skip = "ROOT doens't allocate the array, and if ->Get() is called differently (->Get()->printJSON() instead of assigning an intermediate object), it only *partially* allocates the array"

fill = r"""
struct wrapper {
  char x[10];
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
strcpy(wrap_x.x, "one");
t->Fill();
strcpy(wrap_x.x, "two");
t->Fill();
strcpy(wrap_x.x, "three");
t->Fill();
strcpy(wrap_x.x, "four");
t->Fill();
strcpy(wrap_x.x, "five");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "wrap_x", "type":
            {"type": "record",
             "name": "wrapper",
             "fields": [{"name": "x", "type": "string"}]}
         }]}

json = [{"wrap_x": {"x": "one"}},
        {"wrap_x": {"x": "two"}},
        {"wrap_x": {"x": "three"}},
        {"wrap_x": {"x": "four"}},
        {"wrap_x": {"x": "five"}}]
