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
  TString *fourfour = nullptr;
  TBranch *b_fourfour;
  reader2.GetTree()->SetBranchAddress("fourfour", &fourfour, &b_fourfour);
  std::string *four = nullptr;
  TBranch *b_four;
  reader2.GetTree()->SetBranchAddress("four", &four, &b_four);
  TTreeReaderArray<Float_t> five(reader2, "five");
  TTreeReaderArray<Float_t> six(reader2, "six");
  TTreeReaderArray<Float_t> seven(reader2, "seven");
  TTreeReaderArray<Float_t> sevenseven(reader2, "sevenseven");
  TObjArray *eight = nullptr;
  TBranch *b_eight;
  reader2.GetTree()->SetBranchAddress("eight", &eight, &b_eight);
  while (reader2.Next()) {
    std::cout << *(one.Get()) << " " << *(two.Get()) << " " << (char *)three.GetAddress() << std::endl;

    b_fourfour->GetEntry(reader2.GetCurrentEntry());
    std::cout << "    >" << fourfour->Data() << "<" << std::endl;

    b_four->GetEntry(reader2.GetCurrentEntry());
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

    std::cout << "    ";
    for (int i = 0;  i < sevenseven.GetSize();  i++)
      std::cout << sevenseven[i] << " ";
    std::cout << std::endl;

    // std::cout << "    ";
    // for (int i = 0;  i < sevenseven.GetSize();  i++)
    //   for (int j = 0;  j < sevenseven[i].GetSize();  j++)
    //     std::cout << sevenseven[i][j] << " ";
    // std::cout << std::endl;

    b_eight->GetEntry(reader2.GetCurrentEntry());
    std::cout << "    ";
    for (int i = 0;  i < eight->GetEntries();  i++)
      std::cout << ((TObjString*)eight->At(i))->GetString().Data() << " ";
    std::cout << std::endl;
  }

  std::cout << "file:///home/pivarski/diana-github/scaroot/test-examples/notsosimple.root" << std::endl;

  TFile *file3 = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/notsosimple.root");
  TTreeReader reader3("tree", file3);
  TTreeReaderValue<Double_t> scalar(reader3, "scalar");
  TTreeReaderArray<Double_t> vekkktor(reader3, "vekkktor");
  TTreeReaderArray<std::string> vekkktor2(reader3, "vekkktor2");
  TTreeReaderValue<std::vector<std::vector<double> > > tensor(reader3, "tensor");
  TTreeReaderArray<std::vector<double> > tensor2(reader3, "tensor");
  while (reader3.Next()) {
    std::cout << *(scalar.Get()) << std::endl;
    for (int i = 0;  i < vekkktor.GetSize();  i++) {
      std::cout << vekkktor[i] << " ";
    }
    std::cout << std::endl;
    for (int i = 0;  i < vekkktor2.GetSize();  i++) {
      std::cout << vekkktor2[i] << " ";
    }
    std::cout << std::endl;
    std::vector<std::vector<double> > item = *(tensor.Get());
    for (int i = 0;  i < item.size();  i++) {
      for (int j = 0;  j < item[i].size();  j++) {
        std::cout << item[i][j] << " ";
      }
    }
    std::cout << std::endl;
    for (int i = 0;  i < tensor2.GetSize();  i++) {
      auto tmp = tensor2[i];
      for (int j = 0;  j < tmp.size();  j++) {
        std::cout << tmp[j] << " ";
      }
    }
    std::cout << std::endl;
  }
}
