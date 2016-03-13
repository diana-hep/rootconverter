treeType = TreeType(StdString)

fill = r"""
TTree *t = new TTree("t", "");
std::string x;
t->Branch("x", &x);

x = std::string("one");
t->Fill();

x = std::string("two");
t->Fill();

x = std::string("three");
t->Fill();

x = std::string("four");
t->Fill();

x = std::string("five");
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
