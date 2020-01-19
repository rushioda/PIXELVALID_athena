if [ -e main.C ]; then
    rm main.C
fi

cat <<EOF >> main.C
{
gSystem->Load("libCintex.so");
Cintex::Enable();

std::string incpath = gSystem->GetIncludePath();
incpath += "-I${TestArea}/InstallArea/include/TriggerMenuNtuple";
gSystem->SetIncludePath(incpath.c_str());
gSystem->Load("${TestArea}/InstallArea/${CMTCONFIG}/lib/libTriggerMenuNtupleDict.so");

gROOT->LoadMacro("TrigMuon.C+");
gROOT->LoadMacro("example.C+");

example("ntuple/trigntuple.root");
}
EOF
