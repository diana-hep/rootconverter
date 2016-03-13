treeType = TreeType(CharBrackets(10))

fill = r"""
TTree *t = new TTree("t", "");
char x[10];
t->Branch("x", &x, "x/C");
strcpy(x, "one");
t->Fill();
strcpy(x, "two");
t->Fill();
strcpy(x, "three");
t->Fill();
strcpy(x, "four");
t->Fill();
strcpy(x, "five");
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
