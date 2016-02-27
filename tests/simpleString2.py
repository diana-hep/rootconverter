treeType = TreeType(CharStar)

skip = "this doesn't make retrievable strings (try t->Scan() to see what I mean)"

fill = r"""
TTree *t = new TTree("t", "");
const char *x;
t->Branch("x", &x, "x/C");

std::string one("one");
x = one.c_str();
t->Fill();

std::string two("two");
x = two.c_str();
t->Fill();

std::string three("three");
x = three.c_str();
t->Fill();

std::string four("four");
x = four.c_str();
t->Fill();

std::string five("five");
x = five.c_str();
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": "string"}]}

json = [{"x": "one"},
        {"x": "two"},
        {"x": "three"},
        {"x": "four"},
        {"x": "five"}]
