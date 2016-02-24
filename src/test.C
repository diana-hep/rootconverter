void test() {
  // TFile *file = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/verysimple.root");
  // TTree *tree;
  // file->GetObject("ntuple", tree);
  // Float_t x;
  // Float_t y;
  // Float_t z;
  // tree->SetBranchAddress("x", &x);
  // tree->SetBranchAddress("y", &y);
  // tree->SetBranchAddress("z", &z);
  // for (int i = 0;  i < tree->GetEntries();  i++) {
  //   tree->GetEntry(i);
  //   std::cout << "  " << x << " " << y << " " << z << std::endl;
  // }

  std::cout << "file:///home/pivarski/diana-github/scaroot/test-examples/verysimple.root" << std::endl;

  TFile *file1 = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/verysimple.root");
  TTreeReader reader1("ntuple", file1);
  TTreeReaderValue<Float_t> x(reader1, "x");
  TTreeReaderValue<Float_t> y(reader1, "y");
  TTreeReaderValue<Float_t> z(reader1, "z");
  while (reader1.Next()) {
    std::cout << *(x.Get()) << " " << *(y.Get()) << " " << *(z.Get()) << std::endl;
  }

  // TFile *file = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/simple.root");
  // TTree *tree;
  // file->GetObject("tree", tree);
  // Int_t one;
  // Float_t two;
  // Char_t three[10];
  // tree->SetBranchAddress("one", &one);
  // tree->SetBranchAddress("two", &two);
  // tree->SetBranchAddress("three", &three);
  // for (int i = 0;  i < tree->GetEntries();  i++) {
  //   tree->GetEntry(i);
  //   std::cout << "  " << one << " " << two << " " << three << std::endl;
  // }

  std::cout << "file:///home/pivarski/diana-github/scaroot/test-examples/simple.root" << std::endl;

  TFile *file2 = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/simple.root");
  TTreeReader reader2("tree", file2);
  TTreeReaderValue<Int_t> one(reader2, "one");
  TTreeReaderValue<Float_t> two(reader2, "two");
  TTreeReaderArray<Char_t> three(reader2, "three");
  TString *fourfour = new TString;
  reader2.GetTree()->SetBranchAddress("fourfour", &fourfour);
  std::string *four = new std::string;
  reader2.GetTree()->SetBranchAddress("four", &four);
  TTreeReaderArray<Float_t> five(reader2, "five");
  TTreeReaderArray<Float_t> six(reader2, "six");
  TTreeReaderArray<Float_t> seven(reader2, "seven");
  // TTreeReaderArray<TTreeReaderArray<Float_t> > sevenseven(reader2, "sevenseven");
  TObjArray *eight = new TObjArray;
  reader2.GetTree()->SetBranchAddress("eight", &eight);
  while (reader2.Next()) {
    reader2.GetTree()->GetEntry(reader2.GetCurrentEntry());

    std::cout << *(one.Get()) << " " << *(two.Get()) << " " << (char *)three.GetAddress() << std::endl;

    std::cout << "    >" << fourfour->Data() << "<" << std::endl;
    std::cout << "    >" << *four << "<" << std::endl;

    std::cout << "    ";
    for (int i = 0;  i < five.GetSize();  i++)
      std::cout << five[i] << " ";
    std::cout << std::endl;

    std::cout << "    ";
    for (int i = 0;  i < six.GetSize();  i++)
      std::cout << six[i] << " ";
    std::cout << std::endl;

    std::cout << "    ";
    for (int i = 0;  i < seven.GetSize();  i++)
      std::cout << seven[i] << " ";
    std::cout << std::endl;

    // std::cout << "    ";
    // for (int i = 0;  i < sevenseven.GetSize();  i++)
    //   for (int j = 0;  j < sevenseven[i].GetSize();  j++)
    //     std::cout << sevenseven[i][j] << " ";
    // std::cout << std::endl;

    std::cout << "    ";
    for (int i = 0;  i < eight->GetEntries();  i++)
      std::cout << ((TObjString*)eight->At(i))->GetString().Data() << " ";
    std::cout << std::endl;
  }

  std::cout << "file:///home/pivarski/diana-github/scaroot/test-examples/notsosimple.root" << std::endl;

  TFile *file3 = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/notsosimple.root");
  TTreeReader reader3("tree", file3);
  TTreeReaderValue<Double_t> scalar(reader3, "scalar");
  TTreeReaderArray<Double_t> vector(reader3, "vector");
  TTreeReaderArray<std::string> vector2(reader3, "vector2");
  TTreeReaderArray<std::vector<Double_t> > tensor(reader3, "tensor");
  while (reader3.Next()) {
    std::cout << *(scalar.Get()) << std::endl;
    for (int i = 0;  i < vector.GetSize();  i++) {
      std::cout << vector[i] << " ";
    }
    std::cout << std::endl;
    for (int i = 0;  i < vector2.GetSize();  i++) {
      std::cout << vector2[i] << " ";
    }
    std::cout << std::endl;
    for (int i = 0;  i < tensor.GetSize();  i++) {
      for (int j = 0;  j < tensor[i].size();  j++) {
        std::cout << tensor[i][j] << " ";
      }
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }
}
