/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoCommon/Exception.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace TXC;

L1TopoMenu::L1TopoMenu() {}

L1TopoMenu::L1TopoMenu(const std::string & name, const std::string & version) :
   m_name(name),
   m_version(version),
   m_smk(0)
{}  


L1TopoMenu::L1TopoMenu(L1TopoMenu&& o) :
   m_name( std::move(o.m_name) ),
   m_version( std::move(o.m_version) ),
   m_smk( std::move(o.m_smk) ),
   m_algos( std::move(o.m_algos) ),
   m_topoconfig( std::move(o.m_topoconfig) ),
   m_outputlist( std::move(o.m_outputlist) )
{}


L1TopoMenu::~L1TopoMenu() {}

L1TopoMenu& 
L1TopoMenu::operator=(L1TopoMenu&& o) {
   m_name = std::move(o.m_name);
   m_version = std::move(o.m_version);
   m_smk = std::move(o.m_smk);
   m_algos = std::move(o.m_algos);
   m_topoconfig = std::move(o.m_topoconfig);
   m_outputlist = std::move(o.m_outputlist);
   return *this;
}

void
L1TopoMenu::addAlgorithm(const TXC::L1TopoConfigAlg &alg) {
   m_algos.push_back(alg);
}

const L1TopoConfigAlg &
L1TopoMenu::algorithm(const std::string& name) const {
   for(const L1TopoConfigAlg & alg: m_algos)
      if(alg.name() == name) return alg;
   TCS_EXCEPTION("L1TopoMenu: algorithm of name " << name << " doesn't exist");
}


TCS::StatusCode
L1TopoMenu::setTriggerList() {

   for(const OutputListElement & out: m_outputlist.getOutputList() ) {
      
      const L1TopoConfigAlg & alg = algorithm( out.algoname() );
      
      if(! alg.isDecAlg()) {
         TCS_EXCEPTION("L1TopoMenu: output list contains an algorithm " << out.algoname() << " that is not an decision algorithm");
      }

      unsigned int bit = out.firstbit();
      for (const string& outputname : alg.getOutputNames() ) {
         m_outputlist.addTriggerLine( TriggerLine(outputname, out.algoname(),
                                                  out.algoid(), out.module(), out.fpga(), out.clock(),
                                                  bit++) );
      }
   }

   m_outputlist.sort();

   return TCS::StatusCode::SUCCESS;
}

// print menu
void TXC::L1TopoMenu::print() const {

   cout << "==========================================" << endl
        << "Menu Name   : " << getName() << endl
        << "Menu Version: " << getVersion() << endl
        << m_topoconfig << endl;

   for(const TXC::L1TopoConfigAlg & alg: getL1TopoConfigAlgs())
      cout << alg << endl;

   cout << m_outputlist << endl;

}

void TXC::L1TopoMenu::writeXML(const std::string& filename){
   std::ofstream xmlfile;
   xmlfile.open( filename );
   xmlfile << "<?xml version=\"1.0\" ?>\n"
        << "<TOPO_MENU menu_name=\"Topo_pp_vX\" menu_version=\"1\">\n"
        << "  <!--File is generated by TriggerMenu-->\n"
        << "  <!--No. L1Topo algos defined: " << m_outputlist.getOutputList().size() << "-->\n"
        << "  <OutputList>\n";
        for(auto& out : m_outputlist.getOutputList()) {
           string triggerline = "";
           const L1TopoConfigAlg & alg = algorithm( out.algoname() );
           if(! alg.isDecAlg())
              TCS_EXCEPTION("L1TopoMenu: output list contains an algorithm " << out.algoname() << " that is not an decision algorithm");
           for (const string& outputname : alg.getOutputNames() ) {
              if(triggerline != "") triggerline += ",";
              triggerline += outputname;
           }
           xmlfile << "    <Output algname=\"" << out.algoname() << "\" triggerline=\"" << triggerline << "\" algoId=\"" << out.algoid() << "\" module=\"" << out.module() << "\" fpga=\"" << out.fpga() << "\" firstbit=\"" << out.firstbit() << "\" clock=\"" << out.clock() << "\"/>" << std::endl;
        }
        xmlfile << "  </OutputList>\n\n"
                << "  <TopoConfig>\n";

        for(auto &in : m_topoconfig.getTopoConfigs()){
          xmlfile << "<Entry name=\"" << in.name << "\" value = \"" << in.value << "\"/>" << std::endl;
        }
        xmlfile << "  </TopoConfig>\n\n";

        for(auto& in : m_algos) {
          if(in.isDecAlg()) continue;
          xmlfile << "  <SortAlgo type=\"" << in.type() << "\" name=\"" << in.name() << "\" output=\"" << in.output() << "\" algoId=\"" << in.algoID() << "\">" << std::endl;
          xmlfile << "    <Fixed>" << std::endl;
          for(auto& fin : in.getInputs()){
             xmlfile << "      <Input name=\"" << fin.name << "\" value=\"" << fin.value << "\"/>" << std::endl;
          }
          for(auto& fin : in.getOutputs()){
             xmlfile << "      <Output name=\"" << fin.name << "\" value=\"" << fin.value << "\"/>" << std::endl;
          }
          for(auto& fin : in.getFixedParameters()){
             xmlfile << "      <Generic name=\"" << fin.name << "\" value=\"" << fin.value << "\"/>"<< std::endl;
          }
          xmlfile << "    </Fixed>" << std::endl;
          xmlfile << "    <Variable>" << std::endl;
          int i = 0;
          for(auto& fin : in.getParameters()){
             xmlfile << "      <Parameter pos=\"" << i++ << "\" name=\"" << fin.name << "\" value=\"" << fin.value << "\"/>"<< std::endl;
          }
          xmlfile << "    </Variable>" << std::endl;
          xmlfile << "  </SortAlgo>\n" << std::endl;
        }

        for(auto& in : m_algos){
          if(in.isSortAlg()) continue;
          xmlfile << "  <DecisionAlgo type=\"" << in.type() << "\" name=\"" << in.name() << "\" algoId=\"" << in.algoID() << "\">" << std::endl;
          xmlfile << "    <Fixed>" << std::endl;
          int isize = in.getInputs().size();
          for(auto& fin : in.getInputs()){
              if(isize>1)
                 xmlfile << "      <Input name=\"" << fin.name << "\" value=\"" << fin.value << "\" position=\"" << fin.position << "\"/>" << std::endl;
               else
                 xmlfile << "      <Input name=\"" << fin.name << "\" value=\"" << fin.value << "\"/>" << std::endl;
          }
          bool first=true;
          for(auto& fin : in.getOutputs()){
             if(first) {
                xmlfile << "      <Output name=\"" << fin.name << "\" bits=\"" << in.getOutputs().size() << "\">" << std::endl;
                first = false;
             }
             xmlfile << "        <Bit selection=\"" << fin.position << "\" name=\"" << fin.outname << "\"/>" << std::endl;
          }
          xmlfile << "      </Output>" << std::endl;
          for(auto& fin : in.getFixedParameters()){
             xmlfile << "      <Generic name=\"" << fin.name << "\" value=\"" << fin.value << "\"/>"<< std::endl;
          }
          xmlfile << "    </Fixed>" << std::endl;
          xmlfile << "    <Variable>" << std::endl;
          for(auto& fin : in.getParameters()){
             if((int)fin.selection!=-1) 
                xmlfile << "      <Parameter pos=\"" << fin.position << "\" name=\"" << fin.name << "\" selection=\"" << fin.selection << "\" value=\"" << fin.value << "\"/>"<< std::endl;
             else 
                xmlfile << "      <Parameter pos=\"" << fin.position << "\" name=\"" << fin.name << "\" value=\"" << fin.value << "\"/>"<< std::endl;
          }
          xmlfile << "    </Variable>" << std::endl;
          xmlfile << "  </DecisionAlgo>\n" << std::endl;
        }

        xmlfile << "</TOPO_MENU>\n";
        xmlfile.close();
        return;
}
