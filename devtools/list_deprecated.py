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
COLORS = True

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

class OutputState ():
  
  __STATES = { 'default' : '\033[m',
               'failure' : '\033[31m',
               'success' : '\033[32m' }
  __use_colors = True
  
  def __init__ (self, use_colors=True):
    self.__use_colors = use_colors
  
  def __set_status (self, status):
    if self.__use_colors:
      sys.stdout.write (self.__STATES[status])
  
  def set_failure (self):
    self.__set_status ('failure')
  def set_error (self):
    self.set_failure ()
  def set_success (self):
    self.__set_status ('success')
  def set_ok (self):
    self.set_success ()
  def reset (self):
    self.__set_status ('default')
    

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

class DeprecatedEntry ():
  def __init__ (self):
    self.deprecated_line    = 0     # line where @deprecated was found
    self.function_line      = 0     # starting line of the deprecated function
    self.prototype_line     = 0     # prototype line of the deprecated function
    self.is_doc_deprecated  = False # whether the deprecation is in doc
    self.is_code_deprecated = False # whether the deprecation is in code
    self.function           = '???' # deprecated function name
    self.source_file        = '???' # file containing deprecated function
    self.header_file        = '???' # file containing deprecated prototype
    self.n_uses             = 0     # number of uses in the code
    self.uses               = []    # list of (file, line) entries of uses cases

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
        deprecated = DeprecatedEntry ()
        deprecated.source_file = filename
        deprecated.deprecated_line = line_num
        deprecated.is_doc_deprecated = True
        deprecated.function = '???'
        #~ print '%s:%d::%s()' % (deprecated.filename, deprecated.deprecated_line, deprecated.function)
        for line in fp:
          line_num += 1
          res = func_re.match (line)
          if res:
            deprecated.function = res.group (1)
            break
          elif dep_re.match (line):
            raise ValueError ('Unrecognised input at line %d, deprecation found '
                              'before function name.' % line_num)
        deprecated.function_line = line_num
        deprecateds.append (deprecated)
        printv ('  %s:%d::%s() (deprecated at line %d)' %
                (deprecated.source_file, deprecated.function_line,
                 deprecated.function, deprecated.deprecated_line))
      line_num += 1
    fp.close ()
  
  return deprecateds

def check_deprecated (filename, deprecateds):
  n_matches = 0
  
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
        if re.search (dep.function+'[ \t(]', line):
          #~ print 'deprecation found on %s:%d' % (filename, line_num)
          n_matches += 1
          found = False
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
          
          dep.header_file = filename
          dep.prototype_line = line_num
          dep.is_code_deprecated = found
          
          if not dep.is_code_deprecated:
            printerr ('  %s:%d::%s() not marked as deprecated' %
                      (dep.header_file, dep.prototype_line, dep.function))
          else:
            printv ('  OK for %s:%d::%s()' %
                    (dep.header_file, dep.prototype_line, dep.function))
      line_num += 1
  
  return n_matches

def count_deprecated (filename, deprecateds):
  n_matches = 0
  
  try:
    fp = open (filename)
  except IOError, ex:
    raise ex
  else:
    line_num = 1
    for line in fp:
      for dep in deprecateds:
        #~ print dep
        if re.search ('[^a-zA-Z0-9]'+dep.function+'[^a-zA-Z0-9]', line):
          printv ('  Use of deprecated entry %s found at %s:%d' %
                  (dep.function, filename, line_num))
          dep.n_uses += 1
          dep.uses.append ((filename, line_num))
          n_matches += 1
      line_num += 1
  
  return n_matches

def walk_find (deprecateds, dirname, fnames):
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

def walk_check (args, dirname, fnames):
  #~ print dirname
  
  clean_list (fnames)
  ext_re = re.compile (r'.*\.[ch]$')
  for f in fnames:
    if ext_re.match (f):
      #~ print f
      file = os.path.join (dirname, f)
      try:
        args[1] += check_deprecated (file, args[0])
      except IOError, ex:
        printerr ("* I/O error when trying to read '%s':" % file, ex)

def walk_count (deprecateds, dirname, fnames):
  #~ print dirname
  
  clean_list (fnames)
  ext_re = re.compile (r'.*\.[ch]$')
  for f in fnames:
    if ext_re.match (f):
      #~ print f
      file = os.path.join (dirname, f)
      try:
        n = count_deprecated (file, deprecateds)
        printv ('  %d deprecated occurrence(s) in %s' % (n, file))
      except IOError, ex:
        printerr ("* I/O error when trying to read '%s':" % file, ex)

def main (args=()):
  global VERBOSE
  global COLORS
  
  for arg in args[1:]:
    if arg == '-v':
      VERBOSE = True
    elif arg == '-V':
      VERBOSE = False
    elif arg == '-c':
      COLORS = True
    elif arg == '-C':
      COLORS = False
    else:
      break
    args.remove (arg)
  
  root_srcdir = args[1] if len (args) > 1 else 'src'
  root_headir = args[2] if len (args) > 2 else 'include'
  
  deprecateds = []
  print ('Searching for deprecated functions in the documentation...')
  os.path.walk (root_srcdir, walk_find, deprecateds)
  n_deprecateds = len (deprecateds)
  print ('Done, %d deprecation(s) found.' % (n_deprecateds))
  if len (deprecateds) > 0:
    print ('Checking for corresponding code deprecations...')
    args = [deprecateds, 0]
    os.path.walk (root_headir, walk_check, args)
    n_missing_dep = 0
    for dep in deprecateds:
      if not dep.is_code_deprecated:
        n_missing_dep += 1
    print ('Done, %d matches found (%d missing), %d missing deprecation(s).' % 
          (args[1], n_deprecateds - args[1], n_missing_dep))
    
    print ('Searching deprecated uses...')
    os.path.walk (root_srcdir, walk_count, deprecateds)
    os.path.walk (root_headir, walk_count, deprecateds)
    n_uses = 0
    for dep in deprecateds:
      n_uses += dep.n_uses
    print ('Done, %d total deprecated uses (including declaration and definition)' %
           (n_uses))
    
    print
    print (' -- Final results -- ')
    print
    
    output_status = OutputState (COLORS)
    
    max_uses_ok = 2
    for dep in deprecateds:
      uses_ok = True if dep.n_uses <= max_uses_ok else False
      status_ok = True if uses_ok and dep.is_code_deprecated else False
      status = 'OK' if status_ok else 'bad!'
      if status_ok:
        output_status.set_ok ()
      else:
        output_status.set_failure ()
      
      print ('%s(): %s' % (dep.function, status))
      output_status.reset ()
      if not dep.is_code_deprecated:
        print ('  * Not marked as deprecated (should be done at %s:%d)' %
               (dep.header_file, dep.prototype_line))
      if uses_ok:
        printv ('  * Uses are OK, guess the %d use(s) are declaration and/or definition' %
                (dep.n_uses))
      else:
        print ('  * Used %d time(s) (including declaration and definition):' %
               (dep.n_uses))
        for use in dep.uses:
          print ('    used at %s:%d' % (use[0], use[1]))
  
  return 0

if __name__ == '__main__':
  sys.exit (main (sys.argv))

