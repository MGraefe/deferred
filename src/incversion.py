#!/usr/bin/python
# -*- coding: utf-8 -*-


import re
import sys


def findPatternNum():
	if '-major' in sys.argv:
		return 1;
	if '-minor' in sys.argv:
		return 2;
	if '-revision' in sys.argv:
		return 3;
	return 4;

def replaceVersion():
	f = open('version.txt', 'r');
	if f == None:
		print('File not found');
		return 1;
		
	s = f.read();
	f.close();
	
	pattern = '([0-9]+),([0-9]+),([0-9]+),([0-9]+)'

	reObj = re.search(pattern, s);
	if reObj == None:
		print('Pattern not found');
		return 1;

	patternNum = findPatternNum();
	groups = list(reObj.groups());
	groups[patternNum-1] = str(int(groups[patternNum-1]) + 1);

	ns = re.sub(pattern, ','.join(groups), s);
	
	sh = \
	'#ifndef deferred__all__version_H__\n' \
	'#define deferred__all__version_H__\n\n' \
	'#define DEFERREDVERSION ' + ','.join(groups) + '\n' + \
	'#define DEFERREDVERSION_S "' + '.'.join(groups) + '"\n' + \
	'#define DEFERREDVERSION_MAJOR ' + groups[0] + '\n' + \
	'#define DEFERREDVERSION_MINOR ' + groups[1] + '\n' + \
	'#define DEFERREDVERSION_REVISION ' + groups[2] + '\n' + \
	'#define DEFERREDVERSION_BUILD ' + groups[3] + '\n' + \
	'\n#endif\n';
	
	fh = open('version.h', 'w');
	
	fh.write(sh);
	fh.close();
	ft = open('version.txt', 'w');
	ft.write(','.join(groups));
	ft.close();
	
	return 0;


sys.exit(replaceVersion());
