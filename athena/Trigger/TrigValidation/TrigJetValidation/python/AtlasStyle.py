#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import gROOT, gStyle, TStyle, TColor

from array import array

def setAtlasDefaults(webDisplay):
  print 'Applying ATLAS style settings...\n'
  gROOT.Reset();
  atlasStyle = AtlasStyle();
  gROOT.SetStyle('ATLAS')
  gROOT.ForceStyle()

def AtlasStyle():

  atlasStyle = TStyle('ATLAS','Atlas style');

  # use plain black on white colors
  icol = 0  #WHITE
  atlasStyle.SetFrameBorderMode(icol)
  atlasStyle.SetFrameFillColor(icol)
  atlasStyle.SetCanvasBorderMode(icol)
  atlasStyle.SetCanvasColor(icol)
  atlasStyle.SetPadBorderMode(icol)
  atlasStyle.SetPadColor(icol)
  atlasStyle.SetStatColor(icol)
  #atlasStyle.SetFillColor(icol) # don't use: white fill color for *all* objects

  # set the paper & margin sizes
  # atlasStyle.SetPaperSize(20, 26)

  # set margin sizes
  atlasStyle.SetPadTopMargin(0.05)
  atlasStyle.SetPadRightMargin(0.05)
  atlasStyle.SetPadBottomMargin(0.13)
  atlasStyle.SetPadLeftMargin(0.13)

  # set title offsets (for axis label)
  atlasStyle.SetTitleXOffset(1.0)
  atlasStyle.SetTitleYOffset(1.2)

  # use large fonts
  #font = 72 # Helvetica italics
  font = 42 # Helvetica
  tsize = 0.05
  atlasStyle.SetTextFont(font)

  atlasStyle.SetTextSize(tsize)
  atlasStyle.SetLabelFont(font, 'x')
  atlasStyle.SetTitleFont(font, 'x')
  atlasStyle.SetLabelFont(font, 'y')
  atlasStyle.SetTitleFont(font, 'y')
  atlasStyle.SetLabelFont(font, 'z')
  atlasStyle.SetTitleFont(font, 'z')
  
  atlasStyle.SetLabelSize(tsize, 'x')
  atlasStyle.SetTitleSize(tsize, 'x')
  atlasStyle.SetLabelSize(tsize + 0.01, 'y')
  atlasStyle.SetTitleSize(tsize + 0.01, 'y')
  atlasStyle.SetLabelSize(tsize, 'z')
  atlasStyle.SetTitleSize(tsize, 'z')

  # use bold lines and markers
  atlasStyle.SetMarkerStyle(20)
  atlasStyle.SetMarkerSize(1.5) #0.9) #1.2)
  atlasStyle.SetHistLineWidth(2)
  atlasStyle.SetLineWidth(2)
  atlasStyle.SetLineStyleString(2, '[12 12]') # postscript dashes

  # get rid of X error bars 
  #atlasStyle.SetErrorX(0.001)
  # get rid of error bar caps
  atlasStyle.SetEndErrorSize(0.)

  # do not display any of the standard histogram decorations
  atlasStyle.SetOptTitle(0)
  #atlasStyle.SetOptStat(1111)
  atlasStyle.SetOptStat(0)
  #atlasStyle.SetOptFit(1111)
  atlasStyle.SetOptFit(0)

  # put tick marks on top and RHS of plots
  atlasStyle.SetPadTickX(1)
  atlasStyle.SetPadTickY(1)

  return atlasStyle

