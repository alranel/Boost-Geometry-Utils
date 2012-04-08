#!/usr/bin/perl -w
use strict;
use Module::Build::WithXSpp;

my $build = Module::Build::WithXSpp->new(
  module_name     => 'Boost::Geometry::Utils',
  license         => 'perl',
  configure_requires  => {
      'Module::Build'  => '0.38',
  },
  build_requires  => {
      'Test::Deep'  => '0',
  },
  # HAS_BOOL         : stops Perl/lib/CORE/handy.h from doing "#  define bool char" for MSVC
  extra_compiler_flags => [qw(-DHAS_BOOL)],
  # Provides extra C typemaps that are auto-merged
  extra_typemap_modules => {
    'ExtUtils::Typemaps::Default' => '0.05',
  },
  #for MSVC builds
  early_includes => [qw(
    cstring
    cstdlib
    ostream
    )]
);

$build->create_build_script;
