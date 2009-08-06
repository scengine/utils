#!/usr/bin/env python
# -*- encoding: ascii -*-

"""
Checks SCE's deprecated functions

This script searches functions that are deprecated in their documentation and
checks if they are marked as so in the code.
"""

import sys
import os
import re

VERBOSE = False

def printerr (*args):
  for s in args:
    sys.stderr.write (str (s))
  sys.stderr.write ('\n')

def printv (*args):
  global VERBOSE
  if VERBOSE:
    for s in args:
      sys.stdout.write (str (s))
    sys.stdout.write ('\n')

def clean_list (list):
  i = 0
  while True:
    try:
      if list[i][0] == '.':
        #print 'deleting', list[i]
        del list[i]
      else:
        i += 1
    except IndexError:
      break

# FIXME: do a real parsing, not a RE-based one for better detection
def find_deprecated (filename):
  deprecateds = []
  
  try:
    fp = open (filename)
  except IOError, ex:
    raise ex
  else:
    line_num = 1
    dep_re = re.compile (r'.*[\\@]deprecated.*')
    func_re = re.compile (r'[^ ].*[ \t]([a-zA-Z0-9_]+)[ \t]*\([^\)].*')
    for line in fp:
      if dep_re.match (line):
        found_line = line_num
        function = '???'
        #~ print '%s:%d::%s()' % (filename, line_num, function)
        for line in fp:
          line_num += 1
          res = func_re.match (line)
          if res:
            function = res.group (1)
            break
          elif dep_re.match (line):
            raise ValueError ('Unrecognised input at line %d, deprecation found '
                              'before function name.' % line_num)
        printv ('  %s:%d::%s() (deprecated at line %d)' %
                (filename, line_num, function, found_line))
        deprecateds.append (function)
      line_num += 1
    fp.close ()
  
  return deprecateds

def check_deprecated (filename, deprecateds):
  n_matches = 0
  n_wrong = 0
  
  try:
    fp = open (filename)
  except IOError, ex:
    raise ex
  else:
    line_num = 1
    func_re = re.compile (r'.*(?:\)|[ \t])(SCE_GNUC_DEPRECATED)?[ \t]*;')
    for line in fp:
      for dep in deprecateds:
        #~ print dep
        if re.search (dep+'[ \t(]', line):
          #~ print 'deprecation found on %s:%d' % (filename, line_num)
          n_matches += 1
          found = False
          dep_line = line_num
          res = func_re.match (line)
          if res:
            if res.group (1):
              found = True
          else:
            for line in fp:
              line_num += 1
              res = func_re.match (line)
              if res:
                if res.group (1):
                  found = True
                break
          if not found:
            printerr ('  %s:%d::%s() not marked as deprecated' %
                      (filename, dep_line, dep))
            n_wrong += 1
          else:
            printv ('  OK for %s:%d::%s()' % (filename, dep_line, dep))
      line_num += 1
  
  return (n_matches, n_wrong)

def walk (deprecateds, dirname, fnames):
  #~ print dirname
  
  clean_list (fnames)
  ext_re = re.compile (r'.*\.[ch]$')
  for f in fnames:
    if ext_re.match (f):
      #~ print f
      file = os.path.join (dirname, f)
      try:
        deprecateds += find_deprecated (file)
      except IOError, ex:
        printerr ("* I/O error when trying to read '%s':" % file, ex)
      except ValueError, ex:
        printerr ("* Invalid data in '%s':" % file, ex)

def walk2 (args, dirname, fnames):
  #~ print dirname
  
  clean_list (fnames)
  ext_re = re.compile (r'.*\.[ch]$')
  for f in fnames:
    if ext_re.match (f):
      #~ print f
      file = os.path.join (dirname, f)
      try:
        result = check_deprecated (file, args[0])
        args[1] += result[0]
        args[2] += result[1]
      except IOError, ex:
        printerr ("* I/O error when trying to read '%s':" % file, ex)

def main (args=()):
  global VERBOSE
  
  for arg in args[1:]:
    if arg == '-v':
      VERBOSE = True
    elif arg == '-V':
      VERBOSE = False
    else:
      break
    args.remove (arg)
  
  root_srcdir = args[1] if len (args) > 1 else 'src'
  root_headir = args[2] if len (args) > 2 else 'include'
  
  deprecateds = []
  print ('Searching for deprecated functions in the documentation...')
  os.path.walk (root_srcdir, walk, deprecateds)
  n_deprecateds = len (deprecateds)
  print ('Done, %d deprecation(s) found.' % (n_deprecateds))
  if len (deprecateds) > 0:
    print ('Checking for corresponding code deprecations...')
    args = [deprecateds, 0, 0]
    os.path.walk (root_headir, walk2, args)
    print ('Done, %d matches found (%d missing), %d missing deprecation(s).' % 
          (args[1], n_deprecateds - args[1], args[2]))
  
  return 0

if __name__ == '__main__':
  sys.exit (main (sys.argv))

