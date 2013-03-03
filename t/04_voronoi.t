#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;
use Boost::Geometry::Utils qw(polygon_voronoi polygon);

{
    my @expoly = (
       [
        [1,1],
        [15,3],
        [30,2],
        [29,7],
        [3,5],
      ],
      [
        [4,2],
        [5,4],
        [6,3],
      ]
    );
    my $scale = 100000;
    for my $pl (@expoly) { for my $po (@$pl) {$_*=$scale for @$po}}
    
    # not a real test yet - just something to trigger the XS code
    # and give an idea of whether initial C++ interfacing is working
    my $cnt = polygon_voronoi(\@expoly);
    is(scalar(@$cnt), 32, "voronoi graph half-edge count ok: $cnt");
    #diag(join("\n",map scalar(@$_), @$cnt));
    use lib "/home/mike";
    use Slic4rt;
    my $svg = SVGAppend->new('/home/mike/bgu_gtl_vor.svg',{style=>'background-color:#000000;'},'clobber');
    $svg->appendPolygons({style=>"stroke-width:".($scale*0.1).";stroke:blue;fill:none;"},\@expoly);
    $svg->appendPolylines({style=>"stroke-width:".($scale*0.1).";stroke:red;fill:none;"},@$cnt);
}
ok(1);

__END__
