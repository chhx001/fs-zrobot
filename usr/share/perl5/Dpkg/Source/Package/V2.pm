# Copyright © 2008-2011 Raphaël Hertzog <hertzog@debian.org>
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

package Dpkg::Source::Package::V2;

use strict;
use warnings;

our $VERSION = "0.01";

use base 'Dpkg::Source::Package';

use Dpkg;
use Dpkg::Gettext;
use Dpkg::ErrorHandling;
use Dpkg::Compression;
use Dpkg::Source::Archive;
use Dpkg::Source::Patch;
use Dpkg::Exit;
use Dpkg::Source::Functions qw(erasedir is_binary fs_time);
use Dpkg::Vendor qw(run_vendor_hook);
use Dpkg::Control;
use Dpkg::Changelog::Parse;

use POSIX;
use File::Basename;
use File::Temp qw(tempfile tempdir);
use File::Path;
use File::Spec;
use File::Find;
use File::Copy;

our $CURRENT_MINOR_VERSION = "0";

sub init_options {
    my ($self) = @_;
    $self->SUPER::init_options();
    $self->{'options'}{'include_removal'} = 0
        unless exists $self->{'options'}{'include_removal'};
    $self->{'options'}{'include_timestamp'} = 0
        unless exists $self->{'options'}{'include_timestamp'};
    $self->{'options'}{'include_binaries'} = 0
        unless exists $self->{'options'}{'include_binaries'};
    $self->{'options'}{'preparation'} = 1
        unless exists $self->{'options'}{'preparation'};
    $self->{'options'}{'skip_patches'} = 0
        unless exists $self->{'options'}{'skip_patches'};
    $self->{'options'}{'unapply_patches'} = 0
        unless exists $self->{'options'}{'unapply_patches'};
    $self->{'options'}{'skip_debianization'} = 0
        unless exists $self->{'options'}{'skip_debianization'};
    $self->{'options'}{'create_empty_orig'} = 0
        unless exists $self->{'options'}{'create_empty_orig'};
    $self->{'options'}{'auto_commit'} = 0
        unless exists $self->{'options'}{'auto_commit'};
}

sub parse_cmdline_option {
    my ($self, $opt) = @_;
    if ($opt =~ /^--include-removal$/) {
        $self->{'options'}{'include_removal'} = 1;
        return 1;
    } elsif ($opt =~ /^--include-timestamp$/) {
        $self->{'options'}{'include_timestamp'} = 1;
        return 1;
    } elsif ($opt =~ /^--include-binaries$/) {
        $self->{'options'}{'include_binaries'} = 1;
        return 1;
    } elsif ($opt =~ /^--no-preparation$/) {
        $self->{'options'}{'preparation'} = 0;
        return 1;
    } elsif ($opt =~ /^--skip-patches$/) {
        $self->{'options'}{'skip_patches'} = 1;
        return 1;
    } elsif ($opt =~ /^--unapply-patches$/) {
        $self->{'options'}{'unapply_patches'} = 1;
        return 1;
    } elsif ($opt =~ /^--skip-debianization$/) {
        $self->{'options'}{'skip_debianization'} = 1;
        return 1;
    } elsif ($opt =~ /^--create-empty-orig$/) {
        $self->{'options'}{'create_empty_orig'} = 1;
        return 1;
    } elsif ($opt =~ /^--abort-on-upstream-changes$/) {
        $self->{'options'}{'auto_commit'} = 0;
        return 1;
    } elsif ($opt =~ /^--auto-commit$/) {
        $self->{'options'}{'auto_commit'} = 1;
        return 1;
    }
    return 0;
}

sub do_extract {
    my ($self, $newdirectory) = @_;
    my $fields = $self->{'fields'};

    my $dscdir = $self->{'basedir'};

    my $basename = $self->get_basename();
    my $basenamerev = $self->get_basename(1);

    my ($tarfile, $debianfile, %origtar, %seen);
    my $re_ext = $compression_re_file_ext;
    foreach my $file ($self->get_files()) {
        (my $uncompressed = $file) =~ s/\.$re_ext$//;
        error(_g("duplicate files in %s source package: %s.*"), "v2.0",
              $uncompressed) if $seen{$uncompressed};
        $seen{$uncompressed} = 1;
        if ($file =~ /^\Q$basename\E\.orig\.tar\.$re_ext$/) {
            $tarfile = $file;
        } elsif ($file =~ /^\Q$basename\E\.orig-([[:alnum:]-]+)\.tar\.$re_ext$/) {
            $origtar{$1} = $file;
        } elsif ($file =~ /^\Q$basenamerev\E\.debian\.tar\.$re_ext$/) {
            $debianfile = $file;
        } else {
            error(_g("unrecognized file for a %s source package: %s"),
            "v2.0", $file);
        }
    }

    unless ($tarfile and $debianfile) {
        error(_g("missing orig.tar or debian.tar file in v2.0 source package"));
    }

    erasedir($newdirectory);

    # Extract main tarball
    info(_g("unpacking %s"), $tarfile);
    my $tar = Dpkg::Source::Archive->new(filename => "$dscdir$tarfile");
    $tar->extract($newdirectory, no_fixperms => 1,
                  options => [ "--anchored", "--no-wildcards-match-slash",
                               "--exclude", "*/.pc", "--exclude", ".pc" ]);
    # The .pc exclusion is only needed for 3.0 (quilt) and to avoid
    # having an upstream tarball provide a directory with symlinks
    # that would be blindly followed when applying the patches

    # Extract additional orig tarballs
    foreach my $subdir (keys %origtar) {
        my $file = $origtar{$subdir};
        info(_g("unpacking %s"), $file);
        if (-e "$newdirectory/$subdir") {
            warning(_g("required removal of `%s' installed by original tarball"), $subdir);
            erasedir("$newdirectory/$subdir");
        }
        $tar = Dpkg::Source::Archive->new(filename => "$dscdir$file");
        $tar->extract("$newdirectory/$subdir", no_fixperms => 1);
    }

    # Stop here if debianization is not wanted
    return if $self->{'options'}{'skip_debianization'};

    # Extract debian tarball after removing the debian directory
    info(_g("unpacking %s"), $debianfile);
    erasedir("$newdirectory/debian");
    # Exclude existing symlinks from extraction of debian.tar.gz as we
    # don't want to overwrite something outside of $newdirectory due to a
    # symlink
    my @exclude_symlinks;
    my $wanted = sub {
        return if not -l $_;
        my $fn = File::Spec->abs2rel($_, $newdirectory);
        push @exclude_symlinks, "--exclude", $fn;
    };
    find({ wanted => $wanted, no_chdir => 1 }, $newdirectory);
    $tar = Dpkg::Source::Archive->new(filename => "$dscdir$debianfile");
    $tar->extract($newdirectory, in_place => 1,
                  options => [ '--anchored', '--no-wildcards',
                  @exclude_symlinks ]);

    # Apply patches (in a separate method as it might be overriden)
    $self->apply_patches($newdirectory, usage => 'unpack')
        unless $self->{'options'}{'skip_patches'};
}

sub get_autopatch_name {
    return "zz_debian-diff-auto";
}

sub get_patches {
    my ($self, $dir, %opts) = @_;
    $opts{"skip_auto"} = 0 unless defined($opts{"skip_auto"});
    my @patches;
    my $pd = "$dir/debian/patches";
    my $auto_patch = $self->get_autopatch_name();
    if (-d $pd) {
        opendir(DIR, $pd) || syserr(_g("cannot opendir %s"), $pd);
        foreach my $patch (sort readdir(DIR)) {
            # patches match same rules as run-parts
            next unless $patch =~ /^[\w-]+$/ and -f "$pd/$patch";
            next if $opts{"skip_auto"} and $patch eq $auto_patch;
            push @patches, $patch;
        }
        closedir(DIR);
    }
    return @patches;
}

sub apply_patches {
    my ($self, $dir, %opts) = @_;
    $opts{"skip_auto"} = 0 unless defined($opts{"skip_auto"});
    my @patches = $self->get_patches($dir, %opts);
    return unless scalar(@patches);
    my $applied = File::Spec->catfile($dir, "debian", "patches", ".dpkg-source-applied");
    open(APPLIED, '>', $applied) || syserr(_g("cannot write %s"), $applied);
    print APPLIED "# During $opts{'usage'}\n";
    my $timestamp = fs_time($applied);
    foreach my $patch ($self->get_patches($dir, %opts)) {
        my $path = File::Spec->catfile($dir, "debian", "patches", $patch);
        info(_g("applying %s"), $patch) unless $opts{"skip_auto"};
        my $patch_obj = Dpkg::Source::Patch->new(filename => $path);
        $patch_obj->apply($dir, force_timestamp => 1,
                          timestamp => $timestamp,
                          add_options => [ '-E' ]);
        print APPLIED "$patch\n";
    }
    close(APPLIED);
}

sub unapply_patches {
    my ($self, $dir, %opts) = @_;
    my @patches = reverse($self->get_patches($dir, %opts));
    return unless scalar(@patches);
    my $applied = File::Spec->catfile($dir, "debian", "patches", ".dpkg-source-applied");
    my $timestamp = fs_time($applied);
    foreach my $patch (@patches) {
        my $path = File::Spec->catfile($dir, "debian", "patches", $patch);
        info(_g("unapplying %s"), $patch) unless $opts{"quiet"};
        my $patch_obj = Dpkg::Source::Patch->new(filename => $path);
        $patch_obj->apply($dir, force_timestamp => 1, verbose => 0,
                          timestamp => $timestamp,
                          add_options => [ '-E', '-R' ]);
    }
    unlink($applied);
}

sub upstream_tarball_template {
    my ($self) = @_;
    my $ext = "{" . join(",",
        sort map {
            compression_get_property($_, "file_ext")
        } compression_get_list()) . "}";
    return "../" . $self->get_basename() . ".orig.tar.$ext";
}

sub can_build {
    my ($self, $dir) = @_;
    return 1 if $self->find_original_tarballs(include_supplementary => 0);
    return 1 if $self->{'options'}{'create_empty_orig'} and
                $self->find_original_tarballs(include_main => 0);
    return (0, sprintf(_g("no upstream tarball found at %s"),
                       $self->upstream_tarball_template()));
}

sub before_build {
    my ($self, $dir) = @_;
    $self->check_patches_applied($dir) if $self->{'options'}{'preparation'};
}

sub after_build {
    my ($self, $dir) = @_;
    my $applied = File::Spec->catfile($dir, "debian", "patches", ".dpkg-source-applied");
    my $reason = "";
    if (-e $applied) {
        open(APPLIED, "<", $applied) || syserr(_g("cannot read %s"), $applied);
        $reason = <APPLIED>;
        close(APPLIED);
    }
    if ($reason =~ /^# During preparation/ or
        $self->{'options'}{'unapply_patches'}) {
        $self->unapply_patches($dir);
    }
}

sub prepare_build {
    my ($self, $dir) = @_;
    $self->{'diff_options'} = {
        diff_ignore_regexp => $self->{'options'}{'diff_ignore_regexp'} .
                              '|(^|/)debian/patches/.dpkg-source-applied$',
        include_removal => $self->{'options'}{'include_removal'},
        include_timestamp => $self->{'options'}{'include_timestamp'},
        use_dev_null => 1,
    };
    push @{$self->{'options'}{'tar_ignore'}}, "debian/patches/.dpkg-source-applied";
    $self->check_patches_applied($dir) if $self->{'options'}{'preparation'};
    if ($self->{'options'}{'create_empty_orig'} and
        not $self->find_original_tarballs(include_supplementary => 0))
    {
        # No main orig.tar, create a dummy one
        my $filename = $self->get_basename() . ".orig.tar." .
                       $self->{'options'}{'comp_ext'};
        my $tar = Dpkg::Source::Archive->new(filename => $filename);
        $tar->create();
        $tar->finish();
    }
}

sub check_patches_applied {
    my ($self, $dir) = @_;
    my $applied = File::Spec->catfile($dir, "debian", "patches", ".dpkg-source-applied");
    unless (-e $applied) {
        info(_g("patches are not applied, applying them now"));
        $self->apply_patches($dir, usage => 'preparation');
    }
}

sub generate_patch {
    my ($self, $dir, %opts) = @_;
    my ($dirname, $updir) = fileparse($dir);
    my $basedirname = $self->get_basename();
    $basedirname =~ s/_/-/;

    # Identify original tarballs
    my ($tarfile, %origtar);
    my @origtarballs;
    foreach (sort $self->find_original_tarballs()) {
        if (/\.orig\.tar\.$compression_re_file_ext$/) {
            if (defined($tarfile)) {
                error(_g("several orig.tar files found (%s and %s) but only " .
                         "one is allowed"), $tarfile, $_);
            }
            $tarfile = $_;
            push @origtarballs, $_;
            $self->add_file($_);
        } elsif (/\.orig-([[:alnum:]-]+)\.tar\.$compression_re_file_ext$/) {
            $origtar{$1} = $_;
            push @origtarballs, $_;
            $self->add_file($_);
        }
    }

    error(_g("no upstream tarball found at %s"),
          $self->upstream_tarball_template()) unless $tarfile;

    if ($opts{'usage'} eq "build") {
        info(_g("building %s using existing %s"),
             $self->{'fields'}{'Source'}, "@origtarballs");
    }

    # Unpack a second copy for comparison
    my $tmp = tempdir("$dirname.orig.XXXXXX", DIR => $updir);
    push @Dpkg::Exit::handlers, sub { erasedir($tmp) };

    # Extract main tarball
    my $tar = Dpkg::Source::Archive->new(filename => $tarfile);
    $tar->extract($tmp);

    # Extract additional orig tarballs
    foreach my $subdir (keys %origtar) {
        my $file = $origtar{$subdir};
        $tar = Dpkg::Source::Archive->new(filename => $file);
        $tar->extract("$tmp/$subdir");
    }

    # Copy over the debian directory
    erasedir("$tmp/debian");
    system("cp", "-a", "--", "$dir/debian", "$tmp/");
    subprocerr(_g("copy of the debian directory")) if $?;

    # Apply all patches except the last automatic one
    $opts{'skip_auto'} //= 0;
    $self->apply_patches($tmp, skip_auto => $opts{'skip_auto'}, usage => 'build');

    # Create a patch
    my ($difffh, $tmpdiff) = tempfile($self->get_basename(1) . ".diff.XXXXXX",
                                      DIR => File::Spec->tmpdir(), UNLINK => 0);
    push @Dpkg::Exit::handlers, sub { unlink($tmpdiff) };
    my $diff = Dpkg::Source::Patch->new(filename => $tmpdiff,
                                        compression => "none");
    $diff->create();
    $diff->set_header($self->get_patch_header($dir));
    $diff->add_diff_directory($tmp, $dir, basedirname => $basedirname,
            %{$self->{'diff_options'}},
            handle_binary_func => $opts{'handle_binary'},
            order_from => $opts{'order_from'});
    error(_g("unrepresentable changes to source")) if not $diff->finish();

    if (-s $tmpdiff) {
        info(_g("local changes detected, the modified files are:"));
        my $analysis = $diff->analyze($dir, verbose => 0);
        foreach my $fn (sort keys %{$analysis->{'filepatched'}}) {
            print " $fn\n";
        }
    }

    # Remove the temporary directory
    erasedir($tmp);
    pop @Dpkg::Exit::handlers;
    pop @Dpkg::Exit::handlers;

    return $tmpdiff;
}

sub do_build {
    my ($self, $dir) = @_;
    my @argv = @{$self->{'options'}{'ARGV'}};
    if (scalar(@argv)) {
        usageerr(_g("-b takes only one parameter with format `%s'"),
                 $self->{'fields'}{'Format'});
    }
    $self->prepare_build($dir);

    my $include_binaries = $self->{'options'}{'include_binaries'};
    my @tar_ignore = map { "--exclude=$_" } @{$self->{'options'}{'tar_ignore'}};

    my $sourcepackage = $self->{'fields'}{'Source'};
    my $basenamerev = $self->get_basename(1);

    # Prepare handling of binary files
    my %auth_bin_files;
    my $incbin_file = File::Spec->catfile($dir, "debian", "source", "include-binaries");
    if (-f $incbin_file) {
        open(INC, "<", $incbin_file) || syserr(_g("cannot read %s"), $incbin_file);
        while(defined($_ = <INC>)) {
            chomp; s/^\s*//; s/\s*$//;
            next if /^#/ or /^$/;
            $auth_bin_files{$_} = 1;
        }
        close(INC);
    }
    my @binary_files;
    my $handle_binary = sub {
        my ($self, $old, $new) = @_;
        my $relfn = File::Spec->abs2rel($new, $dir);
        # Include binaries if they are whitelisted or if
        # --include-binaries has been given
        if ($include_binaries or $auth_bin_files{$relfn}) {
            push @binary_files, $relfn;
        } else {
            errormsg(_g("cannot represent change to %s: %s"), $new,
                     _g("binary file contents changed"));
            errormsg(_g("add %s in debian/source/include-binaries if you want" .
                     " to store the modified binary in the debian tarball"),
                     $relfn);
            $self->register_error();
        }
    };
    # Check if the debian directory contains unwanted binary files
    my $unwanted_binaries = 0;
    my $check_binary = sub {
        my $fn = File::Spec->abs2rel($_, $dir);
        if (-f $_ and is_binary($_)) {
            if ($include_binaries or $auth_bin_files{$fn}) {
                push @binary_files, $fn;
            } else {
                errormsg(_g("unwanted binary file: %s"), $fn);
                $unwanted_binaries++;
            }
        }
    };
    my $tar_ignore_glob = "{" . join(",",
        map {
            my $copy = $_;
            $copy =~ s/,/\\,/g;
            $copy;
        } @{$self->{'options'}{'tar_ignore'}}) . "}";
    my $filter_ignore = sub {
        # Filter out files that are not going to be included in the debian
        # tarball due to ignores.
        my %exclude;
        my $reldir = File::Spec->abs2rel($File::Find::dir, $dir);
        my $cwd = getcwd();
        # Apply the pattern both from the top dir and from the inspected dir
        chdir($dir) || syserr(_g("unable to chdir to `%s'"), $dir);
        $exclude{$_} = 1 foreach glob($tar_ignore_glob);
        chdir($cwd) || syserr(_g("unable to chdir to `%s'"), $cwd);
        chdir($File::Find::dir) ||
            syserr(_g("unable to chdir to `%s'"), $File::Find::dir);
        $exclude{$_} = 1 foreach glob($tar_ignore_glob);
        chdir($cwd) || syserr(_g("unable to chdir to `%s'"), $cwd);
        my @result;
        foreach my $fn (@_) {
            unless (exists $exclude{$fn} or exists $exclude{"$reldir/$fn"}) {
                push @result, $fn;
            }
        }
        return @result;
    };
    find({ wanted => $check_binary, preprocess => $filter_ignore,
           no_chdir => 1 }, File::Spec->catdir($dir, "debian"));
    error(P_("detected %d unwanted binary file (add it in " .
             "debian/source/include-binaries to allow its inclusion).",
             "detected %d unwanted binary files (add them in " .
             "debian/source/include-binaries to allow their inclusion).",
             $unwanted_binaries), $unwanted_binaries)
         if $unwanted_binaries;

    # Create a patch
    my $autopatch = File::Spec->catfile($dir, "debian", "patches",
                                        $self->get_autopatch_name());
    my $tmpdiff = $self->generate_patch($dir, order_from => $autopatch,
                                        handle_binary => $handle_binary,
                                        skip_auto => $self->{'options'}{'auto_commit'},
                                        usage => 'build');
    unless (-z $tmpdiff or $self->{'options'}{'auto_commit'}) {
        info(_g("you can integrate the local changes with %s"),
             "dpkg-source --commit");
        error(_g("aborting due to unexpected upstream changes, see %s"),
              $tmpdiff);
    }
    push @Dpkg::Exit::handlers, sub { unlink($tmpdiff) };

    # Install the diff as the new autopatch
    if ($self->{'options'}{'auto_commit'}) {
        mkpath(File::Spec->catdir($dir, "debian", "patches"));
        $autopatch = $self->register_patch($dir, $tmpdiff,
                                           $self->get_autopatch_name());
        info(_g("local changes have been recorded in a new patch: %s"),
             $autopatch) if -e $autopatch;
        rmdir(File::Spec->catdir($dir, "debian", "patches")); # No check on purpose
    }
    unlink($tmpdiff) || syserr(_g("cannot remove %s"), $tmpdiff);
    pop @Dpkg::Exit::handlers;

    # Update debian/source/include-binaries if needed
    if (scalar(@binary_files) and $include_binaries) {
        mkpath(File::Spec->catdir($dir, "debian", "source"));
        open(INC, ">>", $incbin_file) || syserr(_g("cannot write %s"), $incbin_file);
        foreach my $binary (@binary_files) {
            unless ($auth_bin_files{$binary}) {
                print INC "$binary\n";
                info(_g("adding %s to %s"), $binary, "debian/source/include-binaries");
            }
        }
        close(INC);
    }
    # Create the debian.tar
    my $debianfile = "$basenamerev.debian.tar." . $self->{'options'}{'comp_ext'};
    info(_g("building %s in %s"), $sourcepackage, $debianfile);
    my $tar = Dpkg::Source::Archive->new(filename => $debianfile);
    $tar->create(options => \@tar_ignore, 'chdir' => $dir);
    $tar->add_directory("debian");
    foreach my $binary (@binary_files) {
        $tar->add_file($binary) unless $binary =~ m{^debian/};
    }
    $tar->finish();

    $self->add_file($debianfile);
}

sub get_patch_header {
    my ($self, $dir) = @_;
    my $ph = File::Spec->catfile($dir, "debian", "source", "local-patch-header");
    unless (-f $ph) {
        $ph = File::Spec->catfile($dir, "debian", "source", "patch-header");
    }
    my $text;
    if (-f $ph) {
        open(PH, "<", $ph) || syserr(_g("cannot read %s"), $ph);
        $text = join("", <PH>);
        close(PH);
        return $text;
    }
    my $ch_info = changelog_parse(offset => 0, count => 1,
        file => File::Spec->catfile($dir, "debian", "changelog"));
    return '' if not defined $ch_info;
    my $header = Dpkg::Control->new(type => CTRL_UNKNOWN);
    $header->{'Description'} = "<short summary of the patch>\n";
    $header->{'Description'} .=
"TODO: Put a short summary on the line above and replace this paragraph
with a longer explanation of this change. Complete the meta-information
with other relevant fields (see below for details). To make it easier, the
information below has been extracted from the changelog. Adjust it or drop
it.\n";
    $header->{'Description'} .= $ch_info->{'Changes'} . "\n";
    $header->{'Author'} = $ch_info->{'Maintainer'};
    $text = "$header";
    run_vendor_hook("extend-patch-header", \$text, $ch_info);
    $text .= "\n---
The information above should follow the Patch Tagging Guidelines, please
checkout http://dep.debian.net/deps/dep3/ to learn about the format. Here
are templates for supplementary fields that you might want to add:

Origin: <vendor|upstream|other>, <url of original patch>
Bug: <url in upstream bugtracker>
Bug-Debian: http://bugs.debian.org/<bugnumber>
Bug-Ubuntu: https://launchpad.net/bugs/<bugnumber>
Forwarded: <no|not-needed|url proving that it has been forwarded>
Reviewed-By: <name and email of someone who approved the patch>
Last-Update: <YYYY-MM-DD>\n\n";
    return $text;
}

sub register_patch {
    my ($self, $dir, $patch_file, $patch_name) = @_;
    my $patch = File::Spec->catfile($dir, "debian", "patches", $patch_name);
    if (-s $patch_file) {
        copy($patch_file, $patch) ||
            syserr(_g("failed to copy %s to %s"), $patch_file, $patch);
        chmod(0666 & ~ umask(), $patch) ||
                syserr(_g("unable to change permission of `%s'"), $patch);
        my $applied = File::Spec->catfile($dir, "debian", "patches", ".dpkg-source-applied");
        open(APPLIED, '>>', $applied) || syserr(_g("cannot write %s"), $applied);
        print APPLIED "$patch\n";
        close(APPLIED) || syserr(_g("cannot close %s"), $applied);
    } elsif (-e $patch) {
        unlink($patch) || syserr(_g("cannot remove %s"), $patch);
    }
    return $patch;
}

sub commit {
    my ($self, $dir) = @_;
    my ($patch_name, $tmpdiff) = @{$self->{'options'}{'ARGV'}};

    sub bad_patch_name {
        my ($dir, $patch_name) = @_;
        return 1 if not defined($patch_name);
        return 1 if not length($patch_name);
        my $patch = File::Spec->catfile($dir, "debian", "patches", $patch_name);
        if (-e $patch) {
            warning(_g("cannot register changes in %s, this patch already exists"), $patch);
            return 1;
        }
        return 0;
    }

    $self->prepare_build($dir);

    unless ($tmpdiff && -e $tmpdiff) {
        $tmpdiff = $self->generate_patch($dir, usage => "commit");
    }
    push @Dpkg::Exit::handlers, sub { unlink($tmpdiff) };
    unless (-s $tmpdiff) {
        unlink($tmpdiff) || syserr(_g("cannot remove %s"), $tmpdiff);
        info(_g("there are no local changes to record"));
        return;
    }
    while (bad_patch_name($dir, $patch_name)) {
        # Ask the patch name interactively
        print STDOUT _g("Enter the desired patch name: ");
        chomp($patch_name = <STDIN>);
        $patch_name =~ s/\s+/-/g;
        $patch_name =~ s/\///g;
    }
    mkpath(File::Spec->catdir($dir, "debian", "patches"));
    my $patch = $self->register_patch($dir, $tmpdiff, $patch_name);
    system("sensible-editor", $patch);
    unlink($tmpdiff) || syserr(_g("cannot remove %s"), $tmpdiff);
    pop @Dpkg::Exit::handlers;
    info(_g("local changes have been recorded in a new patch: %s"), $patch);
}

# vim:et:sw=4:ts=8
1;
