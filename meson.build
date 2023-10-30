project('Scheduling', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c17'])

incdir = include_directories('include')

exe = executable('scheduler', 'src/scheduler.c',
  include_directories: incdir, install : false)

test('basic', exe)
