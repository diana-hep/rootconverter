treeType = TreeType(Array(StdString, 3))

skip = "ROOT doesn't recognize std::string"

fill = r"""
TTree *t = new TTree("t", "");
std::string x[3];
t->Branch("x", &x);

x[0] = std::string("one");
x[1] = std::string("two");
x[2] = std::string("three");
t->Fill();

x[0] = std::string("uno");
x[1] = std::string("dos");
x[2] = std::string("tres");
t->Fill();

x[0] = std::string("un");
x[1] = std::string("deux");
x[2] = std::string("trois");
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "string"}}]}

json = [{"x": ["one", "two", "three"]},
        {"x": ["uno", "dos", "tres"]},
        {"x": ["un", "deux", "trois"]}]
