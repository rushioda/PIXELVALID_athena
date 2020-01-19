# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ..powheg_V2 import PowhegV2


class Wy(PowhegV2):
    """! Default Powheg configuration for W-boson production with an associated photon.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(Wy, self).__init__(base_directory, "Wgamma", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        self.allowed_decay_modes = ["w- > e- ve~", "w- > mu- vm~", "w- > tau- vt~", "w+ > e+ ve", "w+ > mu+ vm", "w+ > tau+ vt"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphaem")
        self.add_keyword("alphaem0")
        self.add_keyword("bmass_lhe")
        self.add_keyword("bornktmin", 1.0)
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact")
        self.add_keyword("bornzerodamp", 1)
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btildeborn")
        self.add_keyword("btildecoll")
        self.add_keyword("btildereal")
        self.add_keyword("btildevirt")
        self.add_keyword("btlscalect")
        self.add_keyword("btlscalereal")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("check_bad_st2", 1)
        self.add_keyword("CKM_Vcb")
        self.add_keyword("CKM_Vcd")
        self.add_keyword("CKM_Vcs")
        self.add_keyword("CKM_Vtb")
        self.add_keyword("CKM_Vtd")
        self.add_keyword("CKM_Vts")
        self.add_keyword("CKM_Vub")
        self.add_keyword("CKM_Vud")
        self.add_keyword("CKM_Vus")
        self.add_keyword("clobberlhe")
        self.add_keyword("cmass_lhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("Deltak_gam")
        self.add_keyword("deltar")
        self.add_keyword("doublefsr")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("factsc2min", -1)
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("fixedscale")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 5)
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy", 2)
        self.add_keyword("frensc2min", -1)
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("G_mu")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("icsimax")
        self.add_keyword("idvecbos")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 5)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("kt2minqed")
        self.add_keyword("Lambda_gam")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhapdf6maxsets")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")
        self.add_keyword("max_io_bufsize")
        self.add_keyword("maxseeds")
        self.add_keyword("minlo_nnll")
        self.add_keyword("minlo", 1)
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("modept2gamlep")
        self.add_keyword("ncall1", 250000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 1600000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("nlotestonly")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 100000)
        self.add_keyword("olddij", 1)
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("phspseparation")
        self.add_keyword("powheg-c-lo")
        self.add_keyword("powheg-nc")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("runningscale")
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("semileptonic")
        self.add_keyword("skipextratests")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("sthw2")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("vdecaymode", self.allowed_decay_modes[0], name="decay_mode")
        self.add_keyword("withdamp", 1)
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("Wmass")
        self.add_keyword("Wwidth")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 10)
        self.add_keyword("Zmass")
        self.add_keyword("Zwidth")

    def validate_decays(self):
        """! Validate idvecbos and vdecaymode keywords."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        self.parameters_by_keyword("idvecbos")[0].value = 24 * [-1, +1][self.decay_mode.startswith("w+")]
        __decay_mode_lookup = {"e- ve~": 1, "mu- vm~": 2, "tau- vt~": 3, "e+ ve": 1, "mu+ vm": 2, "tau+ vt": 3}
        self.parameters_by_keyword("vdecaymode")[0].value = __decay_mode_lookup[self.decay_mode.split("> ")[1]]
