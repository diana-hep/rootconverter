treeType = TreeType(Struct(StdString))

fill = r"""
struct wrapper {
  std::string x;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);
wrap_x.x = std::string("one");
t->Fill();
wrap_x.x = std::string("two");
t->Fill();
wrap_x.x = std::string("three");
t->Fill();
wrap_x.x = std::string("four");
t->Fill();
wrap_x.x = std::string("five");
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
