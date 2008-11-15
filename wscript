#! /usr/bin/env python
# encoding: utf-8

import sys, os
import Utils, Options

import preproc
preproc.go_absolute = 1

# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='netwars'

# these variables are mandatory ('/' are converted automatically)
srcdir = 'src'
blddir = 'build'

def init():
	pass

def set_options(opt):
	opt.tool_options('g++')
	return

def configure(conf):
	conf.check_tool('g++')
	conf.sub_config('src')
	
	return 

def build(bld):
	bld.add_subdirs('src')
	return

