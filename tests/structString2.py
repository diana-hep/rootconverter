treeType = TreeType(Struct(CharStar))

skip = "wrapped char* string pointer probably isn't supported"

fill = r"""
struct wrapper {
  char *x;
};

TTree *t = new TTree("t", "");
wrapper wrap_x;

t->Branch("wrap_x", &wrap_x);

std::string one("one");
strcpy(wrap_x.x, one.c_str());
t->Fill();

std::string two("two");
strcpy(wrap_x.x, two.c_str());
t->Fill();

std::string three("three");
strcpy(wrap_x.x, three.c_str());
t->Fill();

std::string four("four");
strcpy(wrap_x.x, four.c_str());
t->Fill();

std::string five("five");
strcpy(wrap_x.x, five.c_str());
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
