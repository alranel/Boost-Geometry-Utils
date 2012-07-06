#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Boost::Geometry::Utils qw(polygon linestring polygon_linestring_intersection);

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
        my $intersection = polygon_linestring_intersection($polygon, [$linestring]);
use Data::Dumper; print STDERR Data::Dumper->Dump([$intersection],[qw/i1/]);
        is_deeply $intersection, [
            [ [10, 15], [14, 15] ],
            [ [16, 15], [20, 15] ],
        ], 'line is clipped to square with hole';
    }
    {
        my $intersection = polygon_linestring_intersection($polygon, [$linestring2]);
use Data::Dumper; print STDERR Data::Dumper->Dump([$intersection],[qw/i2/]);
        is_deeply $intersection, [], 'external line produces no intersections';
    }
    {
        my $intersection = polygon_linestring_intersection($polygon, $multilinestring);
use Data::Dumper; print STDERR Data::Dumper->Dump([$intersection],[qw/i3/]);
        is_deeply $intersection, [
            [ [10, 15], [14, 15] ],
            [ [16, 15], [20, 15] ],
        ], 'multiple linestring clipping';
    }
}

__END__
