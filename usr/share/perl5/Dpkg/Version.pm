# Copyright © Colin Watson <cjwatson@debian.org>
# Copyright © Ian Jackson <iwj@debian.org>
# Copyright © 2007 Don Armstrong <don@donarmstrong.com>.
# Copyright © 2009 Raphaël Hertzog <hertzog@debian.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

package Dpkg::Version;

use strict;
use warnings;

our $VERSION = "1.00";

use Dpkg::ErrorHandling;
use Dpkg::Gettext;

use base qw(Exporter);
our @EXPORT = qw(version_compare version_compare_relation
                 version_normalize_relation version_compare_string
                 version_compare_part version_split_digits version_check
                 REL_LT REL_LE REL_EQ REL_GE REL_GT);

use constant {
    REL_LT => '<<',
    REL_LE => '<=',
    REL_EQ => '=',
    REL_GE => '>=',
    REL_GT => '>>',
};

use overload
    '<=>' => \&comparison,
    'cmp' => \&comparison,
    '""'  => \&as_string,
    'bool' => sub { return $_[0]->as_string() if $_[0]->is_valid(); },
    'fallback' => 1;

=encoding utf8

=head1 NAME

Dpkg::Version - handling and comparing dpkg-style version numbers

=head1 DESCRIPTION

The Dpkg::Version module provides pure-Perl routines to compare
dpkg-style version numbers (as used in Debian packages) and also
an object oriented interface overriding perl operators
to do the right thing when you compare Dpkg::Version object between
them.

=head1 OBJECT INTERFACE

=over 4

=item my $v = Dpkg::Version->new($version, %opts)

Create a new Dpkg::Version object corresponding to the version indicated in
the string (scalar) $version. By default it will accepts any string
and consider it as a valid version. If you pass the option "check => 1",
it will return undef if the version is invalid (see version_check for
details).

You can always call $v->is_valid() later on to verify that the version is
valid.

=cut

sub new {
    my ($this, $ver, %opts) = @_;
    my $class = ref($this) || $this;
    $ver = "$ver" if ref($ver); # Try to stringify objects

    if ($opts{'check'}) {
	return undef unless version_check($ver);
    }

    my $self = {};
    if ($ver =~ /^([^:]*):(.+)$/) {
	$self->{'epoch'} = $1;
	$ver = $2;
    } else {
	$self->{'epoch'} = 0;
	$self->{'no_epoch'} = 1;
    }
    if ($ver =~ /(.*)-(.*)$/) {
	$self->{'version'} = $1;
	$self->{'revision'} = $2;
    } else {
	$self->{'version'} = $ver;
	$self->{'revision'} = 0;
	$self->{'no_revision'} = 1;
    }

    return bless $self, $class;
}

=item boolean evaluation

When the Dpkg::Version object is used in a boolean evaluation (for example
in "if ($v)" or "$v || 'default'") it returns its string representation
if the version stored is valid ($v->is_valid()) and undef otherwise.

=item $v->is_valid()

Returns true if the version is valid, false otherwise.

=cut

sub is_valid {
    my ($self) = @_;
    return scalar version_check($self);
}

=item $v->epoch(), $v->version(), $v->revision()

Returns the corresponding part of the full version string.

=cut

sub epoch {
    my $self = shift;
    return $self->{'epoch'};
}

sub version {
    my $self = shift;
    return $self->{'version'};
}

sub revision {
    my $self = shift;
    return $self->{'revision'};
}

=item $v1 <=> $v2, $v1 < $v2, $v1 <= $v2, $v1 > $v2, $v1 >= $v2

Numerical comparison of various versions numbers. One of the two operands
needs to be a Dpkg::Version, the other one can be anything provided that
its string representation is a version number.

=cut

sub comparison {
    my ($a, $b, $inverted) = @_;
    if (not ref($b) or not $b->isa("Dpkg::Version")) {
        $b = Dpkg::Version->new($b);
    }
    ($a, $b) = ($b, $a) if $inverted;
    my $r = version_compare_part($a->epoch(), $b->epoch());
    return $r if $r;
    $r = version_compare_part($a->version(), $b->version());
    return $r if $r;
    return version_compare_part($a->revision(), $b->revision());
}

=item "$v", $v->as_string()

Returns the string representation of the version number.

=cut

sub as_string {
    my ($self) = @_;
    my $str = "";
    $str .= $self->{epoch} . ":" unless $self->{no_epoch};
    $str .= $self->{version};
    $str .= "-" . $self->{revision} unless $self->{no_revision};
    return $str;
}

=back

=head1 FUNCTIONS

All the functions are exported by default.

=over 4

=item version_compare($a, $b)

Returns -1 if $a is earlier than $b, 0 if they are equal and 1 if $a
is later than $b.

If $a or $b are not valid version numbers, it dies with an error.

=cut

sub version_compare($$) {
    my ($a, $b) = @_;
    my $va = Dpkg::Version->new($a, check => 1);
    defined($va) || error(_g("%s is not a valid version"), "$a");
    my $vb = Dpkg::Version->new($b, check => 1);
    defined($vb) || error(_g("%s is not a valid version"), "$b");
    return $va <=> $vb;
}

=item version_compare_relation($a, $rel, $b)

Returns the result (0 or 1) of the given comparison operation. This
function is implemented on top of version_compare().

Allowed values for $rel are the exported constants REL_GT, REL_GE,
REL_EQ, REL_LE, REL_LT. Use version_normalize_relation() if you
have an input string containing the operator.

=cut

sub version_compare_relation($$$) {
    my ($a, $op, $b) = @_;
    my $res = version_compare($a, $b);

    if ($op eq REL_GT) {
	return $res > 0;
    } elsif ($op eq REL_GE) {
	return $res >= 0;
    } elsif ($op eq REL_EQ) {
	return $res == 0;
    } elsif ($op eq REL_LE) {
	return $res <= 0;
    } elsif ($op eq REL_LT) {
	return $res < 0;
    } else {
	internerr("unsupported relation for version_compare_relation(): '$op'");
    }
}

=item my $rel = version_normalize_relation($rel_string)

Returns the normalized constant of the relation $rel (a value
among REL_GT, REL_GE, REL_EQ, REL_LE and REL_LT). Supported
relations names in input are: "gt", "ge", "eq", "le", "lt", ">>", ">=",
"=", "<=", "<<". ">" and "<" are also supported but should not be used as
they are obsolete aliases of ">=" and "<=".

=cut

sub version_normalize_relation($) {
    my $op = shift;

    warning("relation %s is deprecated: use %s or %s",
            $op, "$op$op", "$op=") if ($op eq '>' or $op eq '<');

    if ($op eq '>>' or $op eq 'gt') {
	return REL_GT;
    } elsif ($op eq '>=' or $op eq 'ge' or $op eq '>') {
	return REL_GE;
    } elsif ($op eq '=' or $op eq 'eq') {
	return REL_EQ;
    } elsif ($op eq '<=' or $op eq 'le' or $op eq '<') {
	return REL_LE;
    } elsif ($op eq '<<' or $op eq 'lt') {
	return REL_LT;
    } else {
	internerr("bad relation '$op'");
    }
}

=item version_compare_string($a, $b)

String comparison function used for comparing non-numerical parts of version
numbers. Returns -1 if $a is earlier than $b, 0 if they are equal and 1 if $a
is later than $b.

The "~" character always sort lower than anything else. Digits sort lower
than non-digits. Among remaining characters alphabetic characters (A-Za-z)
sort lower than the other ones. Within each range, the ASCII decimal value
of the character is used to sort between characters.

=cut

sub version_compare_string($$) {
    sub order {
        my ($x) = @_;
	if ($x eq '~') {
	    return -1;
	} elsif ($x =~ /^\d$/) {
	    return $x * 1 + 1;
	} elsif ($x =~ /^[A-Za-z]$/) {
	    return ord($x);
	} else {
	    return ord($x) + 256;
	}
    }
    my @a = map(order($_), split(//, shift));
    my @b = map(order($_), split(//, shift));
    while (1) {
        my ($a, $b) = (shift @a, shift @b);
        return 0 if not defined($a) and not defined($b);
        $a ||= 0; # Default order for "no character"
        $b ||= 0;
        return 1 if $a > $b;
        return -1 if $a < $b;
    }
}

=item version_compare_part($a, $b)

Compare two corresponding sub-parts of a version number (either upstream
version or debian revision).

Each parameter is split by version_split_digits() and resulting items
are compared together.in digits and non-digits items that are compared
together. As soon as a difference happens, it returns -1 if $a is earlier
than $b, 0 if they are equal and 1 if $a is later than $b.

=cut

sub version_compare_part($$) {
    my @a = version_split_digits(shift);
    my @b = version_split_digits(shift);
    while (1) {
        my ($a, $b) = (shift @a, shift @b);
        return 0 if not defined($a) and not defined($b);
        $a ||= 0; # Default value for lack of version
        $b ||= 0;
        if ($a =~ /^\d+$/ and $b =~ /^\d+$/) {
            # Numerical comparison
            my $cmp = $a <=> $b;
            return $cmp if $cmp;
        } else {
            # String comparison
            my $cmp = version_compare_string($a, $b);
            return $cmp if $cmp;
        }
    }
}

=item my @items = version_split_digits($version)

Splits a string in items that are each entirely composed either
of digits or of non-digits. For instance for "1.024~beta1+svn234" it would
return ("1", ".", "024", "~beta", "1", "+svn", "234").

=cut

sub version_split_digits($) {
    return split(/(?<=\d)(?=\D)|(?<=\D)(?=\d)/, $_[0]);
}

=item my ($ok, $msg) = version_check($version)

=item my $ok = version_check($version)

Checks the validity of $version as a version number. Returns 1 in $ok
if the version is valid, 0 otherwise. In the latter case, $msg
contains a description of the problem with the $version scalar.

=cut

sub version_check($) {
    my $version = shift;
    my $str;
    if (defined $version) {
        $str = "$version";
        $version = Dpkg::Version->new($str) unless ref($version);
    }
    if (not defined($str) or not length($str)) {
        my $msg = _g("version number cannot be empty");
        return (0, $msg) if wantarray;
        return 0;
    }
    if ($version->version() =~ m/^[^\d]/) {
        my $msg = _g("version number does not start with digit");
        return (0, $msg) if wantarray;
        return 0;
    }
    if ($str =~ m/([^-+:.0-9a-zA-Z~])/o) {
        my $msg = sprintf(_g("version number contains illegal character `%s'"), $1);
        return (0, $msg) if wantarray;
        return 0;
    }
    if ($version->epoch() !~ /^\d*$/) {
        my $msg = sprintf(_g("epoch part of the version number " .
                             "is not a number: '%s'"), $version->epoch());
        return (0, $msg) if wantarray;
        return 0;
    }
    return (1, "") if wantarray;
    return 1;
}

=back

=head1 AUTHOR

Don Armstrong <don@donarmstrong.com>, Colin Watson
<cjwatson@debian.org> and Raphaël Hertzog <hertzog@debian.org>, based on
the implementation in C<dpkg/lib/vercmp.c> by Ian Jackson and others.

=cut

1;
