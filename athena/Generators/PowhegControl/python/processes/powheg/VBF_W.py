# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ..external import ExternalVBFNLO
from ..powheg_V1 import PowhegV1


class VBF_W(PowhegV1):
    """! Default Powheg configuration for W-boson production through vector boson fusion.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(VBF_W, self).__init__(base_directory, "VBF_W-Z", **kwargs)

        # Add algorithms to the sequence
        self.add_algorithm(ExternalVBFNLO("W", ["w+ > e+ ve", "w+ > mu+ vm", "w- > e- ve~", "w- > mu- vm~"]))

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("bornktmin")
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact")
        self.add_keyword("bornzerodamp")
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btlscalect")
        self.add_keyword("btlscalereal")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("colltest")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fakevirt")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 2)
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy", 2)
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx2", 5)
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("manyseeds")
        self.add_keyword("ncall1", 200000)
        self.add_keyword("ncall2", 3000000)
        self.add_keyword("nubound", 200000)
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("pdfreweight")
        self.add_keyword("Phasespace")
        self.add_keyword("ptj_gencut")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("ptsuppfact", 10)
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp", 1)
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xupbound", 2)
