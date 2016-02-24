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

  // TFile *file = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/verysimple.root");
  // TTreeReader reader("ntuple", file);
  // TTreeReaderValue<Float_t> x(reader, "x");
  // TTreeReaderValue<Float_t> y(reader, "y");
  // TTreeReaderValue<Float_t> z(reader, "z");
  // while (reader.Next()) {
  //   std::cout << *(x.Get()) << " " << *(y.Get()) << " " << *(z.Get()) << std::endl;
  // }

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

  TFile *file = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/simple.root");
  TTreeReader reader("tree", file);
  TTreeReaderValue<Int_t> one(reader, "one");
  TTreeReaderValue<Float_t> two(reader, "two");
  TTreeReaderArray<Char_t> three(reader, "three");
  // TTreeReaderValue<TString*> fourfour(reader, "fourfour");
  // TTreeReaderValue<std::string> four(reader, "four");
  TTreeReaderArray<Float_t> five(reader, "five");
  TTreeReaderArray<Float_t> six(reader, "six");
  TTreeReaderArray<Float_t> seven(reader, "seven");
  // TTreeReaderValue<TObjArray> eight(reader, "eight");
  while (reader.Next()) {
    std::cout << *(one.Get()) << " " << *(two.Get()) << " " << (char *)three.GetAddress() << std::endl;
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
  }


  // Int_t one;
  // Float_t two;
  // Char_t three[10];
  // TString four;
  // Float_t five[2];
  // Float_t six[4];
  // Float_t seven[2][2];
  // TObjArray eight;

  // tree->Branch("one", &one, "one/I");
  // tree->Branch("two", &two, "two/F");
  // tree->Branch("three", &three, "three/C");
  // tree->Branch("four", &four, 16000, 0);
  // tree->Branch("five", &five, "five[2]/F");
  // tree->Branch("six", &six, "six[one]/F");
  // tree->Branch("seven", &seven, "seven[2][2]/F");
  // tree->Branch("eight", &eight, 16000, 0);







  // TFile *file = TFile::Open("file:///home/pivarski/diana-github/scaroot/test-examples/notsosimple.root");
  // TTreeReader reader("tree", file);
  // TTreeReaderValue<Double_t> scalar(reader, "scalar");
  // TTreeReaderArray<Double_t> vector(reader, "vector");
  // TTreeReaderArray<std::string> vector2(reader, "vector2");
  // TTreeReaderArray<std::vector<Double_t> > tensor(reader, "tensor");
  // while (reader.Next()) {
  //   std::cout << *(scalar.Get()) << std::endl;
  //   for (int i = 0;  i < vector.GetSize();  i++) {
  //     std::cout << vector[i] << " ";
  //   }
  //   std::cout << std::endl;
  //   for (int i = 0;  i < vector2.GetSize();  i++) {
  //     std::cout << vector2[i] << " ";
  //   }
  //   std::cout << std::endl;
  //   for (int i = 0;  i < tensor.GetSize();  i++) {
  //     for (int j = 0;  j < tensor[i].size();  j++) {
  //       std::cout << tensor[i][j] << " ";
  //     }
  //   }
  //   std::cout << std::endl;
  //   std::cout << std::endl;
  // }
}
