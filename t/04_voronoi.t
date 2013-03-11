#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Boost::Geometry::Utils qw(polygon_medial_axis);
use List::Util qw(first);
use Math::Clipper qw(orientation);

use constant SOURCE_INDEX => 0;
use constant POINT    => 1;
use constant TWIN     => 2;
use constant NEXT     => 3;
use constant PREVIOUS => 4;
use constant THETA    => 5;
use constant PHI      => 6;
use constant CURVED   => 7;
use constant PRIMARY  => 8;
use constant INTERNAL => 9;
use constant VISITED  => 10;

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
    my @expoly2 = (
        [
          [-182.85714,446.6479],
          [-260,603.79075],
          [8.7692453,559.2707],
          [42.984311,535.98704],
          [77.351978,547.91037],
          [205.71429,526.6479],
          [648.57143,558.07647],
          [632.62232,466.55294],
          [586.18633,452.35022],
          [527.66215,446.38339],
          [479.69269,472.14026],
          [422.48205,469.05075],
          [421.86286,437.5833],
          [459.53403,440.19799],
          [498.13387,402.18796],
          [549.57083,398.3973],
          [591.75,398.35323],
          [625.24287,413.3201],
          [628.57143,306.6479],
        ],
        [
          [-140,483.79075],
          [-22.857143,489.50504],
          [-165.71429,540.93361],
          [-122.85714,506.6479],
        ],
        [
          [94.285714,446.6479],
          [137.14286,438.07647],
          [214.28571,429.50504],
          [274.28571,420.93361],
          [337.14286,412.36218],
          [340,435.21933],
          [325.71429,455.21933],
          [280,455.21933],
          [248.57143,452.36218],
          [211.42857,449.50504],
          [202.85714,466.6479],
          [228.57143,469.50504],
          [271.42857,472.36218],
          [300,478.07647],
          [320,472.36218],
          [348.57143,478.07647],
          [348.57143,506.6479],
          [100,483.79075],
        ]
    );
    
    orient_expolygon(\@expoly2);


    

       
    my $scale  = 100000;
    my $scale2 = 10000;

    for my $pl (@expoly ) { for my $po (@$pl) {$_*=$scale  for @$po}}
    for my $pl (@expoly2) { for my $po (@$pl) {$_*=$scale2 for @$po}}
    
    # Not a real test yet - just something to trigger the XS code
    # and give an idea of whether initial C++ interfacing is working.
    my $ma = polygon_medial_axis(\@expoly2);
    #ok(scalar(@$ma) > 1, "voronoi graph half-edge count ok: ".scalar(@$ma));
    ok($ma->{edges}, "edges is true: ".ref($ma->{edges}). " len: ".scalar(@{$ma->{edges}}));
    ok($ma->{vertices}, "verts is true: ".ref($ma->{vertices}). " len: ".scalar(@{$ma->{edges}}));


    #print "vert0,vert1,internal,primary\n";
    #print join("\n", map {'['.($_->[POINT]? '['.$_->[POINT]->[0].','.$_->[POINT]->[1].']' : 'undef').', '.($_->[TWIN]->[POINT]? '['.$_->[TWIN]->[POINT]->[0].','.$_->[TWIN]->[POINT]->[1].']' : 'undef').', '. ($_->[INTERNAL]).' && '. ($_->[PRIMARY]) . ' next? '.($_->[NEXT] // 'undef')} @{$ma->{edges}}),"\n";


    my @internal_primary_edges    = map [$_->[POINT]//undef,$_->[TWIN]->[POINT]//undef], grep  $_->[INTERNAL] &&  $_->[PRIMARY], @{$ma->{edges}};
    my @internal_nonprimary_edges = map [$_->[POINT]//undef,$_->[TWIN]->[POINT]//undef], grep  $_->[INTERNAL] && !$_->[PRIMARY], @{$ma->{edges}};
    my @external_primary_edges    = map [$_->[POINT]//undef,$_->[TWIN]->[POINT]//undef], grep !$_->[INTERNAL] &&  $_->[PRIMARY], @{$ma->{edges}};
    my @external_nonprimary_edges = map [$_->[POINT]//undef,$_->[TWIN]->[POINT]//undef], grep !$_->[INTERNAL] && !$_->[PRIMARY], @{$ma->{edges}};
    my @degen_int_prim_edges    = ((map $_->[1], grep !$_->[0], @internal_primary_edges),
                                   (map $_->[0], grep !$_->[1], @internal_primary_edges)
                                  );
    my @degen_int_nonprim_edges = ((map $_->[1], grep !$_->[0], @internal_nonprimary_edges),
                                   (map $_->[0], grep !$_->[1], @internal_nonprimary_edges)
                                  );
    my @degen_ext_prim_edges    = ((map $_->[1], grep !$_->[0], @external_primary_edges),
                                   (map $_->[0], grep !$_->[1], @external_primary_edges)
                                  );
    my @degen_ext_nonprim_edges = ((map $_->[1], grep !$_->[0], @external_nonprimary_edges),
                                   (map $_->[0], grep !$_->[1], @external_nonprimary_edges)
                                  );
    @internal_primary_edges    = grep $_->[0] && $_->[1], @internal_primary_edges;
    @internal_nonprimary_edges = grep $_->[0] && $_->[1], @internal_nonprimary_edges;
    @external_primary_edges    = grep $_->[0] && $_->[1], @external_primary_edges;
    @external_nonprimary_edges = grep $_->[0] && $_->[1], @external_nonprimary_edges;

#exit;
    # Collect edge loops going around cells
    my @cell_loops;
    while (my $start_edge = first { not $_->[VISITED] } @{$ma->{edges}}) {
      push @cell_loops, [];
      my $edge = $start_edge;
      do {
        push @{$cell_loops[-1]}, $edge->[POINT] if $edge->[POINT];
        ++$edge->[VISITED];
        $edge = $edge->[NEXT];
        print "UNDEF EDGE\n" if !defined($edge);
      } while ($edge && $edge != $start_edge && !$edge->[VISITED]);
      # close the path
      push @{$cell_loops[-1]}, $edge->[POINT] if $edge->[POINT];
    }

    # reset
    $_->[VISITED] = 0 for @{$ma->{edges}};


#next one worked perfectly a couple times - don't know what I did to mess it up

    # Collect a path of primary edges that doesn't leave the polygon
    my @path_loops;
    if (1) {
    # Sorting on source segment should make the first result loop
    # be the one for the polygon contour, and the rest for the holes.
    # This is probably also preseving opposite winding for countour vs holes
    # but currently the winding convention comes out opposite of the polygon 
    # input winding convention.
    my @source_sorted_edges = sort {$a->[SOURCE_INDEX] <=> $b->[SOURCE_INDEX]} @{$ma->{edges}};
    while (my $start_edge = first { $_->[PRIMARY] and $_->[INTERNAL] and not $_->[VISITED] } @source_sorted_edges) {
       push @path_loops, [];
       my $edge = $start_edge;
       do {
         # collect whatever representation of the edge we want
         # - just points here, but it would be more useful to
         # collect a list of these edge structs.
         push @{$path_loops[-1]}, $edge->[POINT] if $edge->[POINT];
         ++$edge->[VISITED];
         if (!defined $edge->[NEXT]) {print "SAW UNDEF EDGE->NEXT\n";next;}
         if ($edge->[NEXT]->[INTERNAL]) {
           if ($edge->[NEXT]->[PRIMARY]) { # goto next edge within same cell 
             $edge = $edge->[NEXT];
           } else { # skip over a non-primary edge before a curved primary edge
             $edge = $edge->[NEXT]->[TWIN]->[NEXT];
die "now done in c1?"; 
           }
         } else { # corner - end touches polygon, so turn around
           $edge = $edge->[TWIN];                      
die "now done in c2?"; 
         }
       } while (defined $edge->[NEXT] && $edge != $start_edge && not $edge->[VISITED]);
       # Since here we're just collecting points,
       # close the loop with second point of last edge.
       push @{$path_loops[-1]}, $edge->[POINT] if $edge->[POINT];
     }
     }
    
    my @poly = @{$ma->{vertices}};
    


    
    # my @poly2;
    # my %edges;
    # foreach my $e (@{$ma->{edges}}) {
      # $edges{$e} = $e;
    # }
    # do {
      # #//printf("chain\n");
      # my $start_edge = (values %edges)[0];
      # my $walk_edge = $start_edge;
      # do {
        # push(@poly2,$walk_edge->[1]);
        # #//printf("%lu, %lu,  v0,v1: %llu, %llu\n",walk_edge->cell()->source_index(), walk_edge->color(),  walk_edge->vertex0(),walk_edge->vertex1());
        # #                       twin->next
        # $walk_edge = $walk_edge->[2]->[3];
        # #                    previous->twin
        # delete $edges{$walk_edge->[4]->[2]};
      # } while ($walk_edge != $start_edge && scalar(values %edges));
    # } while (scalar(values %edges));
      

    #diag(join("\n",map scalar(@$_), @$ma));
    use lib "/home/mike";
    use Slic4rt;
    my $svg = SVGAppend->new('/home/mike/bgu_gtl_vor.svg',{style=>'background-color:#000000;'},'clobber');
    $svg->appendPolygons({style=>"stroke-width:".($scale*0.1).";stroke:blue;fill:none;"},\@expoly2);
    $svg->appendPolylines({style=>"stroke-width:".($scale*0.1).";stroke:green;fill:none;"},
       @cell_loops);
    $svg->appendPolylines({style=>"stroke-width:".($scale*0.4).";opacity:0.7;stroke-linecap:round;stroke-linejoin:round;stroke:orange;fill:none;"},
       @path_loops);
    $svg->appendCircles({style=>"stroke-width:".($scale*0.05).";stroke:yellow;fill:none;"},
       grep defined($_) && defined($_->[0]) && defined($_->[1] && defined($_->[2])), @poly);
    
    if (0) {
    $svg->appendLines({style=>"stroke-width:".($scale*0.5).";stroke:#0000FF;fill:none;"},
        @external_primary_edges);
    $svg->appendLines({style=>"stroke-width:".($scale*0.5).";stroke:aqua;fill:none;"},
        @external_nonprimary_edges);
    $svg->appendPoints({r=>($scale*0.9),style=>"stroke-width:".($scale*0.05).";stroke:#0000FF;fill:none;"},
       map [$_->[0],$_->[1]], @degen_ext_prim_edges);
    $svg->appendPoints({r=>($scale*0.9),style=>"stroke-width:".($scale*0.05).";stroke:aqua;fill:none;"},
       map [$_->[0],$_->[1]], @degen_ext_nonprim_edges);
    
    $svg->appendLines({style=>"stroke-width:".($scale*0.5).";stroke:#666666;fill:none;"},
        @internal_primary_edges);
    $svg->appendLines({style=>"stroke-width:".($scale*0.5).";stroke:#AAAAAA;fill:none;"},
        @internal_nonprimary_edges);
    $svg->appendPoints({r=>($scale*0.9),style=>"stroke-width:".($scale*0.05).";stroke:#666666;fill:none;"},
       map [$_->[0],$_->[1]], @degen_int_prim_edges);
    $svg->appendPoints({r=>($scale*0.9),style=>"stroke-width:".($scale*0.05).";stroke:#AAAAAA;fill:none;"},
       map [$_->[0],$_->[1]], @degen_int_nonprim_edges);
   }
    # $svg->appendPolylines({style=>"stroke-width:".($scale*0.2).";stroke:orange;fill:none;"},
       # [grep defined($_) && defined($_->[0]), @poly2]);
}
ok(1);


sub orient_expolygon {
  my $expoly = shift;
  if (!orientation($expoly->[0])) {@{$expoly->[0]} = reverse @{$expoly->[0]};}
  foreach my $hole (@{$expoly}[1..$#$expoly]) {
    if (orientation($hole)) {@{$hole} = reverse @{$hole};}
  }
}
    
    
__END__
