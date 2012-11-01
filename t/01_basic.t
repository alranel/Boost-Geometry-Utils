#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 16;
use Boost::Geometry::Utils qw(polygon_multi_linestring_intersection
                              point_within_polygon point_covered_by_polygon
                              linestring_simplify);

{
    my $square = [  # ccw
        [10, 10],
        [20, 10],
        [20, 20],
        [10, 20],
    ];
    my $hole_in_square = [  # cw
        [14, 14],
        [14, 16],
        [16, 16],
        [16, 14],
    ];
    my $polygon = [$square, $hole_in_square];
    my $linestring = [ [5, 15], [30, 15] ];
    my $linestring2 = [ [40, 15], [50, 15] ];  # external
    my $multilinestring = [ [ [5, 15], [30, 15] ], [ [40, 15], [50, 15] ] ];
    
    {
        my $intersection =
            polygon_multi_linestring_intersection($polygon, [$linestring]);
        is_deeply $intersection, [
            [ [10, 15], [14, 15] ],
            [ [16, 15], [20, 15] ],
        ], 'line is clipped to square with hole';
    }
    {
        my $intersection =
            polygon_multi_linestring_intersection($polygon, [$linestring2]);
        is_deeply $intersection, [], 'external line produces no intersections';
    }
    {
        my $intersection =
            polygon_multi_linestring_intersection($polygon, $multilinestring);
        is_deeply $intersection, [
            [ [10, 15], [14, 15] ],
            [ [16, 15], [20, 15] ],
        ], 'multiple linestring clipping';
    }

    {
        my $point_in = [11,11];
        my $point_out = [8,8];
        my $point_in_hole = [15,15];
        my $point_on_edge = [10,15];
        my $point_on_hole_edge = [14,15];
        ok point_within_polygon($point_in, $polygon), 'point in polygon';
        ok !point_within_polygon($point_out, $polygon), 'point outside polygon';
        ok !point_within_polygon($point_in_hole, $polygon),
            'point in hole in polygon';
        my $hole = [$hole_in_square];
        ok point_within_polygon($point_in_hole, $hole), 'point in hole';
        ok !point_within_polygon($point_on_edge, $polygon),
            'point on polygon edge';
        ok !point_within_polygon($point_on_hole_edge, $polygon),
            'point on hole edge';

        ok point_covered_by_polygon($point_in, $polygon), 'point in polygon';
        ok !point_covered_by_polygon($point_out, $polygon),
            'point outside polygon';
        ok !point_covered_by_polygon($point_in_hole, $polygon),
            'point in hole in polygon';
        ok point_covered_by_polygon($point_in_hole, $hole), 'point in hole';
        ok point_covered_by_polygon($point_on_edge, $polygon),
            'point on polygon edge';
        ok point_covered_by_polygon($point_on_hole_edge, $polygon),
            'point on hole edge';
    }

    {
        my $line = [[11, 11], [25, 21], [31, 31], [49, 11], [31, 19]];
        my $simplified = linestring_simplify($line, 5);
        is_deeply $simplified,
            [ [11, 11], [31, 31], [49, 11], [31, 19] ],
            'linestring simplification';
    }
}

__END__
