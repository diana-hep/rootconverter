treeType = TreeType(Vector(Vector(Double_t)))

fill = r"""
TTree *t = new TTree("t", "");
std::vector<std::vector<double> > x;
t->Branch("x", &x);
x = {};
t->Fill();
x = {{}, {}};
t->Fill();
x = {{10}, {}, {10, 20}};
t->Fill();
x = {{20, -21, -22}};
t->Fill();
x = {{200}, {-201}, {202}};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": {"type": "array", "items": "double"}}}]}

json = [{"x": []},
        {"x": [[], []]},
        {"x": [[10], [], [10, 20]]},
        {"x": [[20, -21, -22]]},
        {"x": [[200], [-201], [202]]}]
