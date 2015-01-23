package IPC::Open3;

use strict;
no strict 'refs'; # because users pass me bareword filehandles
our ($VERSION, @ISA, @EXPORT);

require Exporter;

use Carp;
use Symbol qw(gensym qualify);

$VERSION	= 1.09;
@ISA		= qw(Exporter);
@EXPORT		= qw(open3);

# &open3: Marc Horowitz <marc@mit.edu>
# derived mostly from &open2 by tom christiansen, <tchrist@convex.com>
# fixed for 5.001 by Ulrich Kunitz <kunitz@mai-koeln.com>
# ported to Win32 by Ron Schmidt, Merrill Lynch almost ended my career
# fixed for autovivving FHs, tchrist again
# allow fd numbers to be used, by Frank Tobin
# allow '-' as command (c.f. open "-|"), by Adam Spiers <perl@adamspiers.org>
#
# usage: $pid = open3('wtr', 'rdr', 'err' 'some cmd and args', 'optarg', ...);
#
# spawn the given $cmd and connect rdr for
# reading, wtr for writing, and err for errors.
# if err is '', or the same as rdr, then stdout and
# stderr of the child are on the same fh.  returns pid
# of child (or dies on failure).

# if wtr begins with '<&', then wtr will be closed in the parent, and
# the child will read from it directly.  if rdr or err begins with
# '>&', then the child will send output directly to that fd.  In both
# cases, there will be a dup() instead of a pipe() made.

# WARNING: this is dangerous, as you may block forever
# unless you are very careful.
#
# $wtr is left unbuffered.
#
# abort program if
#   rdr or wtr are null
#   a system call fails

our $Me = 'open3 (bug)';	# you should never see this, it's always localized

# Fatal.pm needs to be fixed WRT prototypes.

sub xfork {
    my $pid = fork;
    defined $pid or croak "$Me: fork failed: $!";
    return $pid;
}

sub xpipe {
    pipe $_[0], $_[1] or croak "$Me: pipe($_[0], $_[1]) failed: $!";
}

sub xpipe_anon {
    pipe $_[0], $_[1] or croak "$Me: pipe failed: $!";
}

sub xclose_on_exec {
    require Fcntl;
    my $flags = fcntl($_[0], &Fcntl::F_GETFD, 0)
	or croak "$Me: fcntl failed: $!";
    fcntl($_[0], &Fcntl::F_SETFD, $flags|&Fcntl::FD_CLOEXEC)
	or croak "$Me: fcntl failed: $!";
}

# I tried using a * prototype character for the filehandle but it still
# disallows a bareword while compiling under strict subs.

sub xopen {
    open $_[0], $_[1] or croak "$Me: open($_[0], $_[1]) failed: $!";
}

sub xclose {
    $_[0] =~ /\A=?(\d+)\z/ ? eval { require POSIX; POSIX::close($1); } : close $_[0]
}

sub fh_is_fd {
    return $_[0] =~ /\A=?(\d+)\z/;
}

sub xfileno {
    return $1 if $_[0] =~ /\A=?(\d+)\z/;  # deal with fh just being an fd
    return fileno $_[0];
}

use constant DO_SPAWN => $^O eq 'os2' || $^O eq 'MSWin32';

sub _open3 {
    local $Me = shift;
    my($package, $dad_wtr, $dad_rdr, $dad_err, @cmd) = @_;
    my($dup_wtr, $dup_rdr, $dup_err, $kidpid);

    if (@cmd > 1 and $cmd[0] eq '-') {
	croak "Arguments don't make sense when the command is '-'"
    }

    # simulate autovivification of filehandles because
    # it's too ugly to use @_ throughout to make perl do it for us
    # tchrist 5-Mar-00

    unless (eval  {
	$dad_wtr = $_[1] = gensym unless defined $dad_wtr && length $dad_wtr;
	$dad_rdr = $_[2] = gensym unless defined $dad_rdr && length $dad_rdr;
	1; })
    {
	# must strip crud for croak to add back, or looks ugly
	$@ =~ s/(?<=value attempted) at .*//s;
	croak "$Me: $@";
    }

    $dad_err ||= $dad_rdr;

    $dup_wtr = ($dad_wtr =~ s/^[<>]&//);
    $dup_rdr = ($dad_rdr =~ s/^[<>]&//);
    $dup_err = ($dad_err =~ s/^[<>]&//);

    # force unqualified filehandles into caller's package
    $dad_wtr = qualify $dad_wtr, $package unless fh_is_fd($dad_wtr);
    $dad_rdr = qualify $dad_rdr, $package unless fh_is_fd($dad_rdr);
    $dad_err = qualify $dad_err, $package unless fh_is_fd($dad_err);

    my $kid_rdr = gensym;
    my $kid_wtr = gensym;
    my $kid_err = gensym;

    xpipe $kid_rdr, $dad_wtr if !$dup_wtr;
    xpipe $dad_rdr, $kid_wtr if !$dup_rdr;
    xpipe $dad_err, $kid_err if !$dup_err && $dad_err ne $dad_rdr;

    if (!DO_SPAWN) {
	# Used to communicate exec failures.
	xpipe my $stat_r, my $stat_w;

	$kidpid = xfork;
	if ($kidpid == 0) {  # Kid
	    eval {
		# A tie in the parent should not be allowed to cause problems.
		untie *STDIN;
		untie *STDOUT;

		close $stat_r;
		xclose_on_exec $stat_w;

		# If she wants to dup the kid's stderr onto her stdout I need to
		# save a copy of her stdout before I put something else there.
		if ($dad_rdr ne $dad_err && $dup_err
			&& xfileno($dad_err) == fileno(STDOUT)) {
		    my $tmp = gensym;
		    xopen($tmp, ">&$dad_err");
		    $dad_err = $tmp;
		}

		if ($dup_wtr) {
		    xopen \*STDIN,  "<&$dad_wtr" if fileno(STDIN) != xfileno($dad_wtr);
		} else {
		    xclose $dad_wtr;
		    xopen \*STDIN,  "<&=" . fileno $kid_rdr;
		}
		if ($dup_rdr) {
		    xopen \*STDOUT, ">&$dad_rdr" if fileno(STDOUT) != xfileno($dad_rdr);
		} else {
		    xclose $dad_rdr;
		    xopen \*STDOUT, ">&=" . fileno $kid_wtr;
		}
		if ($dad_rdr ne $dad_err) {
		    if ($dup_err) {
			# I have to use a fileno here because in this one case
			# I'm doing a dup but the filehandle might be a reference
			# (from the special case above).
			xopen \*STDERR, ">&" . xfileno($dad_err)
			    if fileno(STDERR) != xfileno($dad_err);
		    } else {
			xclose $dad_err;
			xopen \*STDERR, ">&=" . fileno $kid_err;
		    }
		} else {
		    xopen \*STDERR, ">&STDOUT" if fileno(STDERR) != fileno(STDOUT);
		}
		return 0 if ($cmd[0] eq '-');
		exec @cmd or do {
		    local($")=(" ");
		    croak "$Me: exec of @cmd failed";
		};
	    };

	    my $bang = 0+$!;
	    my $err = $@;
	    utf8::encode $err if $] >= 5.008;
	    print $stat_w pack('IIa*', $bang, length($err), $err);
	    close $stat_w;

	    eval { require POSIX; POSIX::_exit(255); };
	    exit 255;
	}
	else {  # Parent
	    close $stat_w;
	    my $to_read = length(pack('I', 0)) * 2;
	    my $bytes_read = read($stat_r, my $buf = '', $to_read);
	    if ($bytes_read) {
		(my $bang, $to_read) = unpack('II', $buf);
		read($stat_r, my $err = '', $to_read);
		if ($err) {
		    utf8::decode $err if $] >= 5.008;
		} else {
		    $err = "$Me: " . ($! = $bang);
		}
		$! = $bang;
		die($err);
	    }
	}
    }
    else {  # DO_SPAWN
	# All the bookkeeping of coincidence between handles is
	# handled in spawn_with_handles.

	my @close;
	if ($dup_wtr) {
	  $kid_rdr = \*{$dad_wtr};
	  push @close, $kid_rdr;
	} else {
	  push @close, \*{$dad_wtr}, $kid_rdr;
	}
	if ($dup_rdr) {
	  $kid_wtr = \*{$dad_rdr};
	  push @close, $kid_wtr;
	} else {
	  push @close, \*{$dad_rdr}, $kid_wtr;
	}
	if ($dad_rdr ne $dad_err) {
	    if ($dup_err) {
	      $kid_err = \*{$dad_err};
	      push @close, $kid_err;
	    } else {
	      push @close, \*{$dad_err}, $kid_err;
	    }
	} else {
	  $kid_err = $kid_wtr;
	}
	require IO::Pipe;
	$kidpid = eval {
	    spawn_with_handles( [ { mode => 'r',
				    open_as => $kid_rdr,
				    handle => \*STDIN },
				  { mode => 'w',
				    open_as => $kid_wtr,
				    handle => \*STDOUT },
				  { mode => 'w',
				    open_as => $kid_err,
				    handle => \*STDERR },
				], \@close, @cmd);
	};
	die "$Me: $@" if $@;
    }

    xclose $kid_rdr if !$dup_wtr;
    xclose $kid_wtr if !$dup_rdr;
    xclose $kid_err if !$dup_err && $dad_rdr ne $dad_err;
    # If the write handle is a dup give it away entirely, close my copy
    # of it.
    xclose $dad_wtr if $dup_wtr;

    select((select($dad_wtr), $| = 1)[0]); # unbuffer pipe
    $kidpid;
}

sub open3 {
    if (@_ < 4) {
	local $" = ', ';
	croak "open3(@_): not enough arguments";
    }
    return _open3 'open3', scalar caller, @_
}

sub spawn_with_handles {
    my $fds = shift;		# Fields: handle, mode, open_as
    my $close_in_child = shift;
    my ($fd, $pid, @saved_fh, $saved, %saved, @errs);
    require Fcntl;

    foreach $fd (@$fds) {
	$fd->{tmp_copy} = IO::Handle->new_from_fd($fd->{handle}, $fd->{mode});
	$saved{fileno $fd->{handle}} = $fd->{tmp_copy};
    }
    foreach $fd (@$fds) {
	bless $fd->{handle}, 'IO::Handle'
	    unless eval { $fd->{handle}->isa('IO::Handle') } ;
	# If some of handles to redirect-to coincide with handles to
	# redirect, we need to use saved variants:
	$fd->{handle}->fdopen($saved{fileno $fd->{open_as}} || $fd->{open_as},
			      $fd->{mode});
    }
    unless ($^O eq 'MSWin32') {
	# Stderr may be redirected below, so we save the err text:
	foreach $fd (@$close_in_child) {
	    fcntl($fd, Fcntl::F_SETFD(), 1) or push @errs, "fcntl $fd: $!"
		unless $saved{fileno $fd}; # Do not close what we redirect!
	}
    }

    unless (@errs) {
	$pid = eval { system 1, @_ }; # 1 == P_NOWAIT
	push @errs, "IO::Pipe: Can't spawn-NOWAIT: $!" if !$pid || $pid < 0;
    }

    foreach $fd (@$fds) {
	$fd->{handle}->fdopen($fd->{tmp_copy}, $fd->{mode});
	$fd->{tmp_copy}->close or croak "Can't close: $!";
    }
    croak join "\n", @errs if @errs;
    return $pid;
}

1; # so require is happy
