package Boost::Geometry::Utils;
# ABSTRACT: Bindings for the Boost Geometry library
use strict;
use warnings;

require Exporter;
our @ISA = qw(Exporter);

use XSLoader;
XSLoader::load('Boost::Geometry::Utils', $Boost::Geometry::Utils::VERSION);

our @EXPORT_OK = qw(polygon_to_wkt linestring_to_wkt wkt_to_multilinestring
    polygon linestring polygon_linestring_intersection
    polygon_multi_linestring_intersection);

sub polygon_to_wkt {
    sprintf 'POLYGON(%s)', join ',', map { sprintf '(%s)', join ',', map { join ' ', @$_ } @$_ } @_;
}

sub linestring_to_wkt {
    sprintf "MULTILINESTRING(%s)", join ',', map { sprintf '(%s)', join ',', map { join ' ', @$_ } @$_ } @_;
}

sub wkt_to_multilinestring {
    return [] if $_[0] eq 'MULTILINESTRING()';
    $_[0] =~ s/^MULTILINESTRING\(\(//;
    $_[0] =~ s/\)\)$//;
    [ map [ map [ split / / ], split /,/ ], split /\),\(/, $_[0] ];
}

sub polygon {
    _read_wkt_polygon(polygon_to_wkt(@_));
}

sub linestring {
    _read_wkt_linestring(linestring_to_wkt(@_));
}

1;

=head1 SYNOPSIS

    use Boost::Geometry::Utils qw(polygon linestring polygon_linestring_intersection);
    
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
    my $polygon = polygon($square, $hole_in_square);
    my $linestring = linestring([ [5, 15], [30, 15] ]);
    
    my $intersection = polygon_linestring_intersection($polygon, $linestring);
    
    # $intersection is:
    # [
    #     [ [10, 15], [14, 15] ],
    #     [ [16, 15], [20, 15] ],
    # ]

=head1 ABSTRACT

This module provides bindings to perform some geometric operations using
the Boost Geometry library. It does not aim at providing full bindings
for such library, and that's why I left the I<Boost::Geometry> namespace
free. I'm unsure about the optimal architectural for providing full 
bindings, but I'm interested in such a project -- so, if you have ideas
please get in touch with me.

B<Warning:> the API could change in the future.

=head1 METHODS

=head2 polygon

Converts one or more arrayref(s) of points to a Boost Geometry polygon 
structure. Note that a Boost Geometry polygon is actually a polygon
with holes.

=head2 linestring

Converts an arrayref of points to a Boost Geometry linestring.

=head2 polygon_linestring_intersection

Performs an intersection between the supplied polygon and linestring,
and returns an arrayref of linestrings (represented as arrayrefs of
points).
Note that such an intersection is also called I<clipping>.

=head2 polygon_to_wkt

Converts one or more arrayref(s) of points to a WKT representation of
a polygon (with holes).

=head2 linestring_to_wkt

Converts an arrayref of points to a WKT representation of a multilinestring.

=head2 wkt_to_multilinestring

Parses a MULTILINESTRING back to a Perl data structure.

=head1 ACKNOWLEDGEMENTS

Thanks to mauke and mst (Matt S. Trout (cpan:MSTROUT) <mst@shadowcat.co.uk>)
for their valuable help in getting this to compile under Windows (MinGW) too.

=cut
