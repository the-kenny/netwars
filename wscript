#! /usr/bin/env python
# encoding: utf-8

import sys, os
import Utils, Options

# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='netwars'

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = 'build'

def init():
	pass

def set_options(opt):
	opt.tool_options('g++')
	opt.sub_options('src')
	return

def configure(conf):
	conf.check_tool('g++')
	conf.sub_config('src')
	
	return 

def build(bld):
	bld.add_subdirs('src')
	return

