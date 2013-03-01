#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Boost::Geometry::Utils qw(polygon_voronoi polygon);

{
    my @expoly = (
       [
        [1,1],
        [15,3],
        [30,2],
        [29,7],
        [3,5],
      ]
    );
    
    # not a real test yet - just something to trigger the XS code
    # and give an idea of whether initial C++ interfacing is working
    my $cnt = polygon_voronoi(\@expoly);
    is($cnt, 20, "voronoi graph half-edge count ok: $cnt");
}

__END__
