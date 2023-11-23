project('Scheduling', 'c',
  version : '0.3',
  default_options : ['warning_level=3', 'c_std=c17'])

incdir = include_directories('include')

stc = dependency('stc', version: '4.2')

executable('scheduler', 'src/scheduler/process.c', 
  'src/argparse.c', 'src/scheduler/scheduler.c',
  include_directories: incdir, dependencies: [stc])

executable('memory', 'src/memory/memory.c', 'src/memory/replacer.c',
  include_directories: incdir, dependencies: [stc])