# Copyright © 2007 Raphaël Hertzog <hertzog@debian.org>
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

package Dpkg::Shlibs;

use strict;
use warnings;

our $VERSION = "0.01";

use base qw(Exporter);
our @EXPORT_OK = qw(@librarypaths find_library);

use File::Spec;

use Dpkg::Gettext;
use Dpkg::ErrorHandling;
use Dpkg::Shlibs::Objdump;
use Dpkg::Path qw(resolve_symlink canonpath);
use Dpkg::Arch qw(debarch_to_gnutriplet debarch_to_multiarch
                  get_build_arch get_host_arch);

use constant DEFAULT_LIBRARY_PATH =>
    qw(/lib /usr/lib /lib32 /usr/lib32 /lib64 /usr/lib64
       /emul/ia32-linux/lib /emul/ia32-linux/usr/lib);

# Adjust set of directories to consider when we're in a situation of a
# cross-build or a build of a cross-compiler
my @crosslibrarypaths;
my $crossprefix;
# And when we're not cross-compiling, be sure to pick up the multiarch paths
my @multiarchpaths;
my $multiarch;
# Detect cross compiler builds
if ($ENV{GCC_TARGET}) {
    $crossprefix = debarch_to_gnutriplet($ENV{GCC_TARGET});
}
if ($ENV{DEB_TARGET_GNU_TYPE} and
    ($ENV{DEB_TARGET_GNU_TYPE} ne $ENV{DEB_BUILD_GNU_TYPE}))
{
    $crossprefix = $ENV{DEB_TARGET_GNU_TYPE};
}
# host for normal cross builds.
if (get_build_arch() ne get_host_arch()) {
    $crossprefix = debarch_to_gnutriplet(get_host_arch());
    $multiarch = debarch_to_multiarch(get_host_arch());
}
# Define list of directories containing crossbuilt libraries
if ($crossprefix) {
    push @crosslibrarypaths, "/$crossprefix/lib", "/usr/$crossprefix/lib",
            "/$crossprefix/lib32", "/usr/$crossprefix/lib32",
            "/$crossprefix/lib64", "/usr/$crossprefix/lib64";
}
if ($multiarch) {
    push @multiarchpaths, "/lib/$multiarch", "/usr/lib/$multiarch";
}

our @librarypaths = (@multiarchpaths, DEFAULT_LIBRARY_PATH, @crosslibrarypaths);

# Update library paths with LD_LIBRARY_PATH
if ($ENV{LD_LIBRARY_PATH}) {
    foreach my $path (reverse split( /:/, $ENV{LD_LIBRARY_PATH} )) {
	$path =~ s{/+$}{};
	unshift @librarypaths, $path;
    }
}

# Update library paths with ld.so config
parse_ldso_conf("/etc/ld.so.conf") if -e "/etc/ld.so.conf";

my %visited;
sub parse_ldso_conf {
    my $file = shift;
    open my $fh, "<", $file or syserr(_g("cannot open %s"), $file);
    $visited{$file}++;
    while (<$fh>) {
	next if /^\s*$/;
	chomp;
	s{/+$}{};
	if (/^include\s+(\S.*\S)\s*$/) {
	    foreach my $include (glob($1)) {
		parse_ldso_conf($include) if -e $include
		    && !$visited{$include};
	    }
	} elsif (m{^\s*/}) {
	    s/^\s+//;
	    my $libdir = $_;
	    unless (scalar grep { $_ eq $libdir } @librarypaths) {
		push @librarypaths, $libdir;
	    }
	}
    }
    close $fh;
}

# find_library ($soname, \@rpath, $format, $root)
sub find_library {
    my ($lib, $rpath, $format, $root) = @_;
    $root = "" if not defined($root);
    $root =~ s{/+$}{};
    my @rpath = @{$rpath};
    foreach my $dir (@rpath, @librarypaths) {
	my $checkdir = "$root$dir";
	# If the directory checked is a symlink, check if it doesn't
	# resolve to another public directory (which is then the canonical
	# directory to use instead of this one). Typical example
	# is /usr/lib64 -> /usr/lib on amd64.
	if (-l $checkdir) {
	    my $newdir = resolve_symlink($checkdir);
	    if (grep { "$root$_" eq "$newdir" } (@rpath, @librarypaths)) {
		$checkdir = $newdir;
	    }
	}
	if (-e "$checkdir/$lib") {
	    my $libformat = Dpkg::Shlibs::Objdump::get_format("$checkdir/$lib");
	    if ($format eq $libformat) {
		return canonpath("$checkdir/$lib");
	    }
	}
    }
    return undef;
}

1;
